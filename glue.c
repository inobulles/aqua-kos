
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <signal.h>
#include <stdint.h>
#include <string.h>
#include <dlfcn.h>
#include <dirent.h>

#include "src/iar.h"
#include "src/kos.h"

// functions left to implement

void kos_get_platform       (void) { printf("IMPLEMENT %s\n", __func__); }
void kos_platform_command   (void* zvm, const char* command) { system(command); } /// REMME
void kos_native             (void) { printf("IMPLEMENT %s\n", __func__); }

void kos_create_machine     (void) { printf("IMPLEMENT %s\n", __func__); }
void kos_execute_machine    (void) { printf("IMPLEMENT %s\n", __func__); }
void kos_kill_machine       (void) { printf("IMPLEMENT %s\n", __func__); }
void kos_give_machine_events(void) { printf("IMPLEMENT %s\n", __func__); }
void kos_current_machine    (void) { printf("IMPLEMENT %s\n", __func__); }

#include "zvm/zvm.h"

#define ROOT_PATH "root"
#define BOOT_PATH "root/boot.zpk"

static zvm_program_t* de_program;
static iar_file_t boot_package;

int main(int argc, char** argv) {
	char* root_path = (char*) ROOT_PATH;
	char* boot_path = (char*) BOOT_PATH;
	
	printf("Parsing arguments ...\n");
	for (int i = 1; i < argc; i++) {
		if (strncmp(argv[i], "--", 2) == 0) { // argument is option
			char* option = argv[i] + 2;
			
			if (strcmp(option, "help") == 0) {
				printf("AQUA KOS command-line executable help\n");
				printf("`--help`: Print out help.\n");
				printf("`--root [root directory path]`: Specify where the root directory is.\n");
				printf("`--boot [boot package path]`: Specifiy where the boot package is. Note that this is relative to the working directory, not the root directory.\n");
				return 0;
				
			}
			
			else if (strcmp(option, "root") == 0) root_path = argv[++i];
			else if (strcmp(option, "boot") == 0) boot_path = argv[++i];
			
			else {
				fprintf(stderr, "ERROR Option `--%s` is unknown. Run `aqua --help` to see a list of available options\n", option);
				return 1;
			}
		} else {
			fprintf(stderr, "ERROR Unexpected argument `%s`\n", argv[i]);
			return 1;
		}
	}
	
	printf("Reading the boot package (%s) ...\n", boot_path);
	if (iar_open(&boot_package, boot_path)) return 1;
	
	printf("Finding start node ...\n");
	
	iar_node_t start_node;
	if (iar_find_node(&boot_package, &start_node, "start", &boot_package.root_node) == -1) {
		fprintf(stderr, "ERROR Failed to find start node in boot package\n");
		//iar_free(&boot_package); // don't care about freeing this; no risk for memory leaks
		return 1;
	}
	
	printf("Reading start node ...\n");
	if (!start_node.data_bytes) {
		fprintf(stderr, "ERROR Start node empty\n");
		return 1;
	}
	
	char* start_command = (char*) malloc(start_node.data_bytes);
	if (iar_read_node_contents(&boot_package, &start_node, start_command)) {
		return 1;
	}
	
	if (strncmp(start_command, "zed", 3) == 0) {
		printf("Start command is zed, finding ROM node ...\n");
		
		iar_node_t rom_node;
		if (iar_find_node(&boot_package, &rom_node, "rom.zed", &boot_package.root_node) == -1) {
			fprintf(stderr, "ERROR Failed to find ROM node (rom.zed) in boot package\n");
			return 1;
		}
		
		printf("Reading rom node ...\n");
		if (!rom_node.data_bytes) {
			fprintf(stderr, "ERROR ROM node empty\n");
			return 1;
		}
		
		char* rom = (char*) malloc(rom_node.data_bytes);
		if (iar_read_node_contents(&boot_package, &rom_node, rom)) {
			return 1;
		}
		
		printf("Loading the KOS ...\n");
		load_kos();
		
		printf("Loading the DE ...\n");
		
		de_program = (zvm_program_t*) malloc(sizeof(zvm_program_t));
		memset(de_program, 0, sizeof(zvm_program_t));
		de_program->pointer = rom;
		
		printf("Starting run setup phase ...\n");
		zvm_program_run_setup_phase(de_program);
		
		while (!zvm_program_run_loop_phase(de_program));
		int error_code = de_program->error_code;
		
		zvm_program_free(de_program);
		free(rom);
		
		printf("DE return code is %d\n", error_code);
		
		printf("Quitting KOS ...\n");
		quit_kos();
		free(de_program);
		
		return error_code;
	}
	
	printf("ERROR Unknown start command %s\n", start_command);
	return 1;
}
