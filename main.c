
#if __linux__
	#define _GNU_SOURCE
	#include <sys/mman.h> // for 'memfd_create'
#endif

#include <errno.h>
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
#include <sys/mount.h>

#if defined(__FreeBSD__)
	// TODO perhaps use some of the 'procctl' commands to do stuff like forcing ASLR?
	#include <sys/procctl.h>
#endif

#include <iar.h>

// important global variables

typedef enum {
	KOS_START_NONE = 0,
	KOS_START_ZED, KOS_START_NATIVE, KOS_START_SYSTEM,
	KOS_START_LEN
} kos_start_t;

static kos_start_t kos_start = KOS_START_NONE;
static iar_file_t boot_package;

static char* unique = NULL;
static char* cwd_path = NULL;

static char* root_path = NULL;
static char* boot_path = NULL;

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

// logging

static unsigned verbose = 0;

#define INFO(...) if (verbose) printf("[AQUA KOS] " __VA_ARGS__);
#define WARN(...) fprintf(stderr, "[AQUA KOS] WARNING " __VA_ARGS__);
#define ERROR(...) fprintf(stderr, "[AQUA KOS] ERROR " __VA_ARGS__);

// functions left to implement

void kos_get_platform    (void) { printf("IMPLEMENT %s\n", __func__); }
void kos_platform_command(void* zvm, const char* command) { system(command); } /// REMME
void kos_get_requests    (void* zvm, const char* command) { system(command); } /// REMME
void kos_native          (void) { printf("IMPLEMENT %s\n", __func__); }

#include <zvm.h>
static zvm_program_t* de_program;

// start functions

static int start_nothing(void* data, uint64_t bytes) {
	ERROR("Unknown start command ('%d')\n", kos_start)
	return -1;
}

static int start_zed(void* data, uint64_t bytes) {
	int rv = -1;

	INFO("Loading ZED ROM ...\n")

	de_program = calloc(1, sizeof *de_program);
	de_program->rom = data;

	INFO("Starting run setup phase ...\n")

	if (zvm_program_run_setup_phase(de_program)) {
		ERROR("The ZVM's program setup phase failed\n")
		goto done;
	}

	while (!zvm_program_run_loop_phase(de_program)) {
		// do nothing
	}

	rv = de_program->error_code;
	INFO("Program return code is %d\n", rv)

done:

	zvm_program_free(de_program);
	free(de_program);

	return rv;
}

// TODO perhaps I could combine these functions somehow?

static int process_entry(void) {
	int rv = -1;

	char* entry_path = NULL;
	void* entry_data = NULL;

	iar_node_t entry_node;

	INFO("Looking for entry node ...\n")

	if (iar_find_node(&boot_package, &entry_node, "entry", &boot_package.root_node) < 0) {
		ERROR("Failed to find entry node in boot package\n")
		goto done;
	}

	INFO("Reading entry node ...\n")

	if (!entry_node.data_bytes) {
		ERROR("Entry node empty\n")
		goto done;
	}

	entry_path = malloc(entry_node.data_bytes);

	if (iar_read_node_content(&boot_package, &entry_node, entry_path)) {
		free(entry_path);
		entry_path = NULL;

		goto done;
	}

	// read entry data

	INFO("Entry path is %s, looking for entry data node ...\n", entry_path)

	iar_node_t data_node;

	if (iar_find_node(&boot_package, &data_node, entry_path, &boot_package.root_node) < 0) {
		ERROR("Failed to find entry (%s) in boot package\n", entry_path);
		goto done;
	}

	INFO("Reading entry ...\n")

	if (!data_node.data_bytes) {
		ERROR("Entry data node empty\n");
		goto done;
	}

	entry_data = malloc(data_node.data_bytes);

	if (iar_read_node_content(&boot_package, &data_node, entry_data)) {
		goto done;
	}

	INFO("Setting up devices ...\n")
	setup_devices(); // TODO error handling

	if (root_path) {
		INFO("Changing into root directory ...\n");
		chdir(root_path);
	}

	// actually run the application

	int (*START_LUT[KOS_START_LEN]) (void* data, uint64_t bytes);

	for (unsigned i = 0; i < sizeof(START_LUT) / sizeof(*START_LUT); i++) {
		START_LUT[i] = start_nothing;
	}

	START_LUT[KOS_START_ZED] = start_zed;

	rv = START_LUT[kos_start](entry_data, data_node.data_bytes);

	// free up everything

	INFO("Unloading devices ...\n");
	unload_devices();

done:

	if (entry_data) {
		free(entry_data);
	}

	if (entry_path) {
		free(entry_path);
	}

	return rv;
}

static char* process_start(void) {
	char* start_command = NULL;
	iar_node_t start_node;

	INFO("Looking for start node ...\n")

	if (iar_find_node(&boot_package, &start_node, "start", &boot_package.root_node) < 0) {
		ERROR("Failed to find start node in boot package\n")
		goto done;
	}

	INFO("Reading start node ...\n")

	if (!start_node.data_bytes) {
		ERROR("Start node empty\n")
		goto done;
	}

	start_command = malloc(start_node.data_bytes);

	if (iar_read_node_content(&boot_package, &start_node, start_command)) {
		free(start_command);
		start_command = NULL;

		goto done;
	}

done:

	return start_command;
}

static char* process_unique(void) {
	unique = NULL;

	INFO("Making copy of current working directory path ...\n")
	cwd_path = getcwd(NULL, 0);

	INFO("Looking for unique node ...\n")

	iar_node_t unique_node;

	if (iar_find_node(&boot_package, &unique_node, ZPK_UNIQUE_PATH, &boot_package.root_node) < 0) {
		WARN("Boot package doesn't contain any unique node; the data drive won't be accessible by the application\n")
		goto done;
	}

	INFO("Reading unique node ...\n")

	if (!unique_node.data_bytes) {
		WARN("Unique node empty\n")
		goto done;
	}

	unique = malloc(unique_node.data_bytes);

	if (iar_read_node_content(&boot_package, &unique_node, unique)) {
		free(unique);
		unique = NULL;

		goto done;
	}

	INFO("Unique is %s\n", unique)

	chdir(root_path);
	mkdir("data", 0700);
	chdir("data");
	mkdir(unique, 0700);
	chdir(cwd_path);

done:

	return unique;
}

static char** proc_argv = NULL;

static void process_name(void) {
	char* name = NULL;
	iar_node_t name_node;

	INFO("Looking for name node ...\n")

	if (iar_find_node(&boot_package, &name_node, "name", &boot_package.root_node) < 0) {
		WARN("Boot package doesn't contain any name node\n")
		return;
	}

	INFO("Reading name node ...\n")

	if (!name_node.data_bytes) {
		WARN("Name node empty\n")
		return;
	}

	name = malloc(name_node.data_bytes);

	if (iar_read_node_content(&boot_package, &name_node, name)) {
		free(name);
		name = NULL;

		return;
	}

	// success, set the name of the current process

	proc_argv[0] = name;

#if defined(__FreeBSD__)
	setproctitle("%s", name);
	if (0)
#elif __linux__
	if (prctl(PR_SET_NAME, name, NULL, NULL, NULL) < 0)
#endif
	{
		WARN("Setting the name of the process failed\n")
	}

	// free(name);
}

int main(int argc, char** argv) {
	int rv = -1;
	proc_argv = argv; // for 'process_name' to set the name of our process

	root_path = KOS_DEFAULT_ROOT_PATH;
	boot_path = KOS_DEFAULT_BOOT_PATH;
	device_path = KOS_DEFAULT_DEVICES_PATH;

	kos_argc = 0;
	kos_argv = NULL;

	INFO("Parsing arguments ...\n")

	for (int i = 1; i < argc; i++) {
		if (strncmp(argv[i], "--", 2)) {
			ERROR("Unexpected argument '%s'\n", argv[i])
			goto error;
		}

		char* option = argv[i] + 2;

		if (strcmp(option, "verbose") == 0) {
			verbose = 1;
		}

		else if (strcmp(option, "devices") == 0) {
			device_path = argv[++i];
		}

		else if (strcmp(option, "root") == 0) {
			root_path = argv[++i];
		}

		else if (strcmp(option, "boot") == 0) {
			boot_path = argv[++i];
			i++; // skip the argument we're currently parsing (boot_path)

			// all arguments after boot are program arguments

			kos_argc = argc - i;
			kos_argv = argv + i;

			break;
		}
	}

	if (strcmp(root_path, "NO_ROOT") == 0) {
		root_path = NULL;
	}
	
	INFO("Reading the boot package (%s) ...\n", boot_path)
	
	if (iar_open_read(&boot_package, boot_path)) {
		goto error;
	}

	// look for start node
	
	char* start_command = process_start();

	if (!start_command) {
		goto error_boot_package;
	}

	// look for unique node (only relevant when we have a root path)
	// if found, create a private data directory for it and enter it
	
	if (root_path) {
		unique = process_unique();
	}

	// look for a name node
	// if found, set the name of the process to it

	process_name();

	// what's the start command?

	if (strncmp(start_command, "zed", 3) == 0) {
		kos_start = KOS_START_ZED;
	}

	else if (strncmp(start_command, "native", 6) == 0) {
		kos_start = KOS_START_NATIVE;
	}

	else if (strncmp(start_command, "system", 6) == 0) {
		kos_start = KOS_START_SYSTEM;
	}

	else {
		ERROR("Unknown start command '%s'\n", start_command);
		goto error_unique;
	}

	// look for entry node

	rv = process_entry();

	// if (strncmp(start_command, "zed", 3) == 0) {
	// 	kos_start = KOS_START_ZED;
		
	// 	printf("[AQUA KOS] Start command is 'zed', looking for ROM node ...\n");
		
	// 	iar_node_t rom_node;
	// 	if (iar_find_node(&boot_package, &rom_node, ZPK_ROM_PATH, &boot_package.root_node) < 0) {
	// 		fprintf(stderr, "[AQUA KOS] ERROR Failed to find ROM node (" ZPK_ROM_PATH ") in boot package\n");
	// 		return -1;
	// 	}

	// 	printf("[AQUA KOS] Reading rom node ...\n");
	// 	if (!rom_node.data_bytes) {
	// 		fprintf(stderr, "[AQUA KOS] ERROR ROM node empty\n");
	// 		return -1;
	// 	}

	// 	void* rom = malloc(rom_node.data_bytes);
	// 	if (iar_read_node_content(&boot_package, &rom_node, rom)) {
	// 		return -1;
	// 	}

	// 	printf("[AQUA KOS] Setting up devices ...\n");
	// 	setup_devices();

	// 	if (root_path) {
	// 		printf("[AQUA KOS] Changing into root directory ...\n");
	// 		chdir(root_path);
	// 	}

	// 	printf("[AQUA KOS] Loading the DE ...\n");

	// 	de_program = (zvm_program_t*) malloc(sizeof(zvm_program_t));
	// 	memset(de_program, 0, sizeof(zvm_program_t));
	// 	de_program->rom = rom;

	// 	printf("[AQUA KOS] Starting run setup phase ...\n");
	// 	if (zvm_program_run_setup_phase(de_program)) {
	// 		fprintf(stderr, "[AQUA KOS] ERROR The ZVM's program setup phase failed\n");
	// 		return -1;
	// 	}

	// 	while (!zvm_program_run_loop_phase(de_program));
	// 	int error_code = de_program->error_code;

	// 	zvm_program_free(de_program);
	// 	free(rom);

	// 	printf("[AQUA KOS] DE return code is %d\n", error_code);

	// 	printf("[AQUA KOS] Unloading devices ...\n");
	// 	unload_devices();

	// 	free(de_program);
		
	// 	printf("[AQUA KOS] Done\n");
	// 	return error_code;
	// }

	// else if (strncmp(start_command, "native", 6) == 0) {
	// 	// TODO some platforms are not supported. Support them: https://github.com/google/iree/issues/3845
	// 	// also thanks to https://stackoverflow.com/questions/5053664/dlopen-from-memory for introducing me to 'fdlopen' ❤️
		
	// 	kos_start = KOS_START_NATIVE;
				
	// 	printf("[AQUA KOS] Start command is 'native', looking for native binary node ...\n");

	// 	iar_node_t native_binary_node;
	// 	if (iar_find_node(&boot_package, &native_binary_node, ZPK_NATIVE_BINARY_PATH, &boot_package.root_node) < 0) {
	// 		fprintf(stderr, "[AQUA KOS] ERROR Failed to find native binary node (" ZPK_NATIVE_BINARY_PATH ") in boot package\n");
	// 		return -1;
	// 	}

	// 	if (!native_binary_node.data_bytes) {
	// 		fprintf(stderr, "[AQUA KOS] ERROR Native binary node empty\n");
	// 		return -1;
	// 	}

	// 	void* library = (void*) 0;

	// 	#if defined(__FreeBSD__) // are we running on a supported platform? (i.e. FreeBSD or aquaBSD)
	// 		printf("[AQUA KOS] Creating a shared memory file descriptor for the native binary ...\n");

	// 		int file_descriptor = shm_open(SHM_ANON, O_RDWR, 0);
	// 		ftruncate(file_descriptor, native_binary_node.data_bytes);

	// 		void* native_binary = mmap(NULL, native_binary_node.data_bytes, PROT_WRITE, MAP_SHARED, file_descriptor, 0);

	// 		printf("[AQUA KOS] Reading native binary node ...\n");
	// 		if (iar_read_node_content(&boot_package, &native_binary_node, native_binary)) {
	// 			return -1;
	// 		}

	// 		munmap(native_binary, native_binary_node.data_bytes);

	// 		printf("[AQUA KOS] Dynamically linking native binary ...\n");

	// 		library = fdlopen(file_descriptor, RTLD_LAZY);
	// 		close(file_descriptor);
			
	// 	#elif __linux__ // are we instead running on Linux? (https://github.com/google/iree/issues/3845)
	// 		printf("[AQUA KOS] Creating memory file descriptor for the native binary ...\n");

	// 		if (!unique) {
	// 			fprintf(stderr, "[AQUA KOS] ERROR 'unique' node is required for native binaries\n");
	// 			return -1;
	// 		}

	// 		char* name = (char*) malloc(strlen(unique) + 20 /* strlen("aqua_native_binary_") + 1 */);
	// 		sprintf(name, "aqua_native_binary_%s", unique);

	// 		#if defined(__WSL__)
	// 			printf("[AQUA KOS] Applying special special fix for WSL 💛 ...\n");

	// 			char tmp_file_path[] = "/tmp/aqua-XXXXXXX";
	// 			int file_descriptor = mkstemp(tmp_file_path);

	// 			unlink(tmp_file_path);
	// 		#else
	// 			int file_descriptor = memfd_create(name, 0);
	// 		#endif

	// 		ftruncate(file_descriptor, native_binary_node.data_bytes);
	// 		void* native_binary = mmap(NULL, native_binary_node.data_bytes, PROT_WRITE, MAP_SHARED, file_descriptor, 0);

	// 		printf("[AQUA KOS] Reading native binary node ...\n");
	// 		if (iar_read_node_content(&boot_package, &native_binary_node, native_binary)) {
	// 			return -1;
	// 		}

	// 		munmap(native_binary, native_binary_node.data_bytes);
			
	// 		char fd_name[64]; // likely enough space
	// 		snprintf(fd_name, sizeof(fd_name), "/proc/self/fd/%d", file_descriptor);
			
	// 		library = dlopen(fd_name, RTLD_LAZY);
	// 		close(file_descriptor);
	// 	#else // unsupported platform
	// 		fprintf(stderr, "[AQUA KOS] Running native binary ZPK files is unsupported on this platform (only FreeBSD/aquaBSD and GNU/Linux (and apparently WSL now too) are supported currently)\n");
	// 	#endif

	// 	if (!library) {
	// 		fprintf(stderr, "[AQUA KOS] Failed to link the native binary (%s)\n", dlerror());
	// 		return -1;
	// 	}

	// 	printf("[AQUA KOS] Looking for entry symbol to native binary ...\n");
		
	// 	int (*native_binary_entry) () = dlsym(library, "main");

	// 	if (!native_binary_entry) {
	// 		fprintf(stderr, "[AQUA KOS] ERROR Entry symbol not found\n");
	// 		return -1;
	// 	}

	// 	printf("[AQUA KOS] Setting up devices ...\n");
	// 	setup_devices();

	// 	if (root_path) {
	// 		printf("[AQUA KOS] Changing into root directory ...\n");
	// 		chdir(root_path);
	// 	}

	// 	printf("[AQUA KOS] Looking for 'aqua_set_kos_functions' in the native binary and calling it ...\n");
		
	// 	void (*aqua_set_kos_functions) (
	// 		uint64_t (*_kos_query_device) (uint64_t _, uint64_t name),
	// 		uint64_t (*_kos_send_device) (uint64_t _, uint64_t device, uint64_t command, uint64_t data)) = dlsym(library, "aqua_set_kos_functions");

	// 	if (aqua_set_kos_functions) {
	// 		aqua_set_kos_functions(kos_query_device, kos_send_device);
	// 	}

	// 	printf("[AQUA KOS] Entering into native binary ...\n");
	// 	int error_code = native_binary_entry(kos_query_device, kos_send_device);

	// 	printf("[AQUA KOS] Native binary return code is %d\n", error_code);

	// 	printf("[AQUA KOS] Unloading devices ...\n");
	// 	unload_devices();

	// 	printf("[AQUA KOS] Done\n");
	// 	return error_code;
	// }

done:

	INFO("Done\n")
	return rv;

error_unique:

	if (unique) {
		free(unique);
	}

error_start_command:

	free(start_command);

error_boot_package:

	iar_close(&boot_package);

error:

	goto done;
}
