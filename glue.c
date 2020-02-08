
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <signal.h>
#include <stdint.h>
#include <dlfcn.h>
#include <dirent.h>

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

#define ROM_PATH "rom.zed"
static zvm_program_t* de_program;

static int load_rom(const char* path, char** rom, unsigned long long* bytes) {
	FILE* fp = fopen(path, "rb");
	
	if (!fp) {
		printf("WARNING Could not open ROM file (%s)\n", path);
		return 1;
	
	}
	
	fseek(fp, 0, SEEK_END);
	*bytes = (unsigned long long) ftell(fp);
	rewind(fp);
	
	*rom = (char*) malloc(*bytes);
	fread(*rom, sizeof(char), *bytes, fp);
	fclose(fp);
	
	return 0;
}

int main(int argc, char** argv) {
	printf("Parsing arguments ...\n");
	char* path;
	
	if (argc <= 1) path = (char*) ROM_PATH;
	else           path = argv[1];
	
	printf("Loading the KOS ...\n");
	load_kos();
	
	printf("Loading the DE ...\n");
	
	char* rom = (char*) 0;
	unsigned long long bytes = 0;
	
	int error_code = load_rom(path, &rom, &bytes);
	if (error_code) {
		printf("ERROR Failed to load ROM (error code = %d), aborting ...\n", error_code);
		return error_code;
	}
	
	de_program = (zvm_program_t*) malloc(sizeof(zvm_program_t));
	memset(de_program, 0, sizeof(zvm_program_t));
	de_program->pointer = rom;
	
	printf("Starting run setup phase ...\n");
	zvm_program_run_setup_phase(de_program);
	
	while (!zvm_program_run_loop_phase(de_program));
	error_code = de_program->error_code;
	
	zvm_program_free(de_program);
	free(rom);
	
	printf("DE return code is %d\n", error_code);
	
	printf("Quitting KOS ...\n");
	quit_kos();
	free(de_program);
	
	return error_code;
}
