
#if __linux__
	#define _GNU_SOURCE
	#include <sys/mman.h> // for 'memfd_create'
#endif

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <signal.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>

#include <dirent.h>
#include <sys/stat.h>

#include <iar.h>

// important global variables

typedef enum {
	KOS_START_NONE = 0,
	KOS_START_ZED, KOS_START_NATIVE,
} kos_start_t;

static kos_start_t kos_start = KOS_START_NONE;

static iar_file_t boot_package;
static char* unique = (char*) 0;
static char* cwd_path = (char*) 0;

static char* root_path = (char*) 0;
static char* boot_path = (char*) 0;

static uint32_t kos_argc;
static char** kos_argv;

#include "devices.h"

// compile time macros

#ifndef KOS_DEFAULT_ROOT_PATH
	#define KOS_DEFAULT_ROOT_PATH "root"
#endif

#ifndef KOS_DEFAULT_BOOT_PATH
	#define KOS_DEFAULT_BOOT_PATH "root/boot.zpk"
#endif

#ifndef KOS_DEFAULT_DEVICES_PATH
	#define KOS_DEFAULT_DEVICES_PATH "devices"
#endif

// functions left to implement

void kos_get_platform    (void) { printf("IMPLEMENT %s\n", __func__); }
void kos_platform_command(void* zvm, const char* command) { system(command); } /// REMME
void kos_get_requests    (void* zvm, const char* command) { system(command); } /// REMME
void kos_native          (void) { printf("IMPLEMENT %s\n", __func__); }

#include <zvm.h>
static zvm_program_t* de_program;

int main(int argc, char** argv) {
	root_path = KOS_DEFAULT_ROOT_PATH;
	boot_path = KOS_DEFAULT_BOOT_PATH;
	device_path = KOS_DEFAULT_DEVICES_PATH;

	kos_argc = 0;
	kos_argv = (char**) 0;

	printf("[AQUA KOS] Parsing arguments ...\n");
	for (int i = 1; i < argc; i++) {
		if (strncmp(argv[i], "--", 2) == 0) { // argument is option
			char* option = argv[i] + 2;
			
			if (strcmp(option, "root") == 0) root_path = argv[++i];
			else if (strcmp(option, "devices") == 0) device_path = argv[++i];

			else if (strcmp(option, "boot") == 0) {
				boot_path = argv[++i];
				i++; // skip the argument we're currently parsing (boot_path)

				// all arguments after boot are program arguments

				kos_argc = argc - i;
				kos_argv = argv + i;

				break;
			}

			else {
				fprintf(stderr, "[AQUA KOS] ERROR Option '--%s' is unknown. Check README.md or go to https://github.com/inobulles/aqua-kos/blob/master/README.md to see a list of available options\n", option);
				return -1;
			}

		} else {
			fprintf(stderr, "[AQUA KOS] ERROR Unexpected argument '%s'\n", argv[i]);
			return -1;
		}
	}

	if (strcmp(root_path, "NO_ROOT") == 0) {
		root_path = (char*) 0;
	}
	
	printf("[AQUA KOS] Reading the boot package (%s) ...\n", boot_path);
	if (iar_open_read(&boot_package, boot_path)) return -1;
	
	printf("[AQUA KOS] Finding start node ...\n");
	
	iar_node_t start_node;
	if (iar_find_node(&boot_package, &start_node, "start", &boot_package.root_node) < 0) {
		fprintf(stderr, "[AQUA KOS] ERROR Failed to find start node in boot package\n");
		iar_close(&boot_package);
		return -1;
	}

	printf("[AQUA KOS] Reading start node ...\n");
	if (!start_node.data_bytes) {
		fprintf(stderr, "[AQUA KOS] ERROR Start node empty\n");
		return -1;
	}
	
	char* start_command = (char*) malloc(start_node.data_bytes);
	if (iar_read_node_content(&boot_package, &start_node, start_command)) {
		return -1;
	}
	
	if (root_path) {
		printf("[AQUA KOS] Making copy of current working directory path ...\n");
		cwd_path = getcwd((char*) 0, 0);
		
		printf("[AQUA KOS] Finding unique node ...\n");
		
		iar_node_t unique_node;
		if (iar_find_node(&boot_package, &unique_node, ZPK_UNIQUE_PATH, &boot_package.root_node) == -1) {
			printf("[AQUA KOS] WARNING Boot package doesn't contain any unique node; the data drive won't be accessible by the application\n");
			goto end_unique;
		}
		
		printf("[AQUA KOS] Reading unique node ...\n");
		if (!unique_node.data_bytes) {
			printf("[AQUA KOS] WARNING Unique node empty\n");
			goto end_unique;
		}
		
		unique = (char*) malloc(unique_node.data_bytes);
		if (iar_read_node_content(&boot_package, &unique_node, unique)) {
			free(unique);
			goto end_unique;
		}
		
		printf("[AQUA KOS] Unique is %s\n", unique);
		
		chdir(root_path);
		mkdir("data", 0700);
		chdir("data");
		mkdir(unique, 0700);
		chdir(cwd_path);
	}

end_unique:
	
	if (strncmp(start_command, "zed", 3) == 0) {
		kos_start = KOS_START_ZED;
		printf("[AQUA KOS] Start command is 'zed', finding ROM node ...\n");
		
		iar_node_t rom_node;
		if (iar_find_node(&boot_package, &rom_node, ZPK_ROM_PATH, &boot_package.root_node) == -1) {
			fprintf(stderr, "[AQUA KOS] ERROR Failed to find ROM node (" ZPK_ROM_PATH ") in boot package\n");
			return -1;
		}

		printf("[AQUA KOS] Reading rom node ...\n");
		if (!rom_node.data_bytes) {
			fprintf(stderr, "[AQUA KOS] ERROR ROM node empty\n");
			return -1;
		}
		
		void* rom = malloc(rom_node.data_bytes);
		if (iar_read_node_content(&boot_package, &rom_node, rom)) {
			return -1;
		}

		printf("[AQUA KOS] Setting up devices ...\n");
		setup_devices();

		if (root_path) {
			printf("[AQUA KOS] Changing into root directory ...\n");
			chdir(root_path);
		}

		printf("[AQUA KOS] Loading the DE ...\n");

		de_program = (zvm_program_t*) malloc(sizeof(zvm_program_t));
		memset(de_program, 0, sizeof(zvm_program_t));
		de_program->rom = rom;

		printf("[AQUA KOS] Starting run setup phase ...\n");
		if (zvm_program_run_setup_phase(de_program)) {
			fprintf(stderr, "[AQUA KOS] ERROR The ZVM's program setup phase failed\n");
			return -1;
		}
		
		while (!zvm_program_run_loop_phase(de_program));
		int error_code = de_program->error_code;
		
		zvm_program_free(de_program);
		free(rom);

		printf("[AQUA KOS] DE return code is %d\n", error_code);

		printf("[AQUA KOS] Unloading devices ...\n");
		unload_devices();

		free(de_program);
		
		printf("[AQUA KOS] Done\n");
		return error_code;
	}

	else if (strncmp(start_command, "native", 6) == 0) {
		// TODO some platforms are not supported. Support them: https://github.com/google/iree/issues/3845
		// also thanks to https://stackoverflow.com/questions/5053664/dlopen-from-memory for introducing me to 'fdlopen' ❤️
		
		kos_start = KOS_START_NATIVE;
				
		printf("[AQUA KOS] Start command is 'native', finding native binary node ...\n");

		iar_node_t native_binary_node;
		if (iar_find_node(&boot_package, &native_binary_node, ZPK_NATIVE_BINARY_PATH, &boot_package.root_node) == -1) {
			fprintf(stderr, "[AQUA KOS] ERROR Failed to find native binary node (" ZPK_NATIVE_BINARY_PATH ") in boot package\n");
			return -1;
		}

		if (!native_binary_node.data_bytes) {
			fprintf(stderr, "[AQUA KOS] ERROR Native binary node empty\n");
			return -1;
		}

		void* library = (void*) 0;

		#if defined(__FreeBSD__) // are we running on a supported platform? (i.e. FreeBSD or aquaBSD)
			printf("[AQUA KOS] Creating a shared memory file descriptor for the native binary ...\n");

			int file_descriptor = shm_open(SHM_ANON, O_RDWR, 0);
			ftruncate(file_descriptor, native_binary_node.data_bytes);

			void* native_binary = mmap(NULL, native_binary_node.data_bytes, PROT_WRITE, MAP_SHARED, file_descriptor, 0);

			printf("[AQUA KOS] Reading native binary node ...\n");
			if (iar_read_node_content(&boot_package, &native_binary_node, native_binary)) {
				return -1;
			}

			munmap(native_binary, native_binary_node.data_bytes);

			printf("[AQUA KOS] Dynamically linking native binary ...\n");

			library = fdlopen(file_descriptor, RTLD_LAZY);
			close(file_descriptor);
			
		#elif __linux__ // are we instead running on Linux? (https://github.com/google/iree/issues/3845)
			printf("[AQUA KOS] Creating memory file descriptor for the native binary ...\n");

			if (!unique) {
				fprintf(stderr, "[AQUA KOS] ERROR 'unique' node is required for native binaries\n");
				return -1;
			}

			char* name = (char*) malloc(strlen(unique) + 20 /* strlen("aqua_native_binary_") + 1 */);
			sprintf(name, "aqua_native_binary_%s", unique);

			int file_descriptor = memfd_create(name, 0);
			ftruncate(file_descriptor, native_binary_node.data_bytes);

			void* native_binary = mmap(NULL, native_binary_node.data_bytes, PROT_WRITE, MAP_SHARED, file_descriptor, 0);

			printf("[AQUA KOS] Reading native binary node ...\n");
			if (iar_read_node_content(&boot_package, &native_binary_node, native_binary)) {
				return -1;
			}

			munmap(native_binary, native_binary_node.data_bytes);
			
			char fd_name[64]; // likely enough space
			sprintf(fd_name, "/proc/self/fd/%d", file_descriptor);
			
			library = dlopen(fd_name, RTLD_LAZY);
			close(file_descriptor);
			
		#else // unsupported platform
			fprintf(stderr, "[AQUA KOS] Running native binary ZPK files is unsupported on this platform (only FreeBSD/aquaBSD and GNU/Linux are supported currently)\n");
		#endif

		if (!library) {
			fprintf(stderr, "[AQUA KOS] Failed to link the native binary (%s)\n", dlerror());
			return -1;
		}

		printf("[AQUA KOS] Looking for entry symbol to native binary ...\n");
		
		int (*native_binary_entry) () = dlsym(library, "main");

		if (!native_binary_entry) {
			fprintf(stderr, "[AQUA KOS] ERROR Entry symbol not found\n");
			return -1;
		}

		printf("[AQUA KOS] Setting up devices ...\n");
		setup_devices();

		if (root_path) {
			printf("[AQUA KOS] Changing into root directory ...\n");
			chdir(root_path);
		}

		printf("[AQUA KOS] Looking for 'aqua_set_kos_functions' in the native binary and calling it ...\n");
		
		void (*aqua_set_kos_functions) (
			uint64_t (*_kos_query_device) (uint64_t _, uint64_t name),
			uint64_t (*_kos_send_device) (uint64_t _, uint64_t device, uint64_t command, uint64_t data)) = dlsym(library, "aqua_set_kos_functions");

		if (aqua_set_kos_functions) {
			aqua_set_kos_functions(kos_query_device, kos_send_device);
		}

		printf("[AQUA KOS] Entering into native binary ...\n");
		int error_code = native_binary_entry(kos_query_device, kos_send_device);

		printf("[AQUA KOS] Native binary return code is %d\n", error_code);

		printf("[AQUA KOS] Unloading devices ...\n");
		unload_devices();

		printf("[AQUA KOS] Done\n");
		return error_code;
	}

	printf("[AQUA KOS] ERROR Unknown start command '%s'\n", start_command);
	return -1;
}
