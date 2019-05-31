
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

void mfree(void* ptr, unsigned long long bytes) {
	free(ptr);
	
}

#include "src/kos.h"

static char* first_argv;
#include "src/machine.h"

#include "asm/asm.h"

static kos_t kos;
#define ROM_PATH "ROM.zed" /// TODO make this rom.zed
static program_t* de_program;

static int load_rom(const char* path, char** rom, unsigned long long* bytes) {
	#if KOS_USES_JNI
		if (load_asset_bytes((const char*) __path, rom, bytes)) {
			if (!default_assets) {
				ALOGW("WARNING Could not load the ROM from internal / external storage. Trying from assets ...\n");
				default_assets = true;
				
				if (load_asset_bytes((const char*) __path, rom, bytes)) {
					ALOGE("ERROR Could not load ROM from assets either\n");
					return 1;
					
				}
				
			} else {
				ALOGE("ERROR Could not load the ROM\n");
				return 1;
				
			}
			
		}
	#else
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
	#endif
	
	return 0;
	
}

int main(int argc, char** argv) {
	first_argv = argv[0];
	
	printf("Parsing arguments ...\n");
	char* path;
	
	if (argc <= 1) path = (char*) ROM_PATH;
	else           path = argv[1];
	
	
	if (argc > 2 && argv[2][0] == 'x') {
		printf("Text only machine\n");
		/// TODO is text only machine
		
	} if (argc > 4) {
		/// TODO width  = atoi(argv[3]);
		/// TODO height = atoi(argv[4]);
		
	} if (argc > 5) {
		printf("Child machine (parent PID = %s)\n", argv[6]);
		/// TODO is child machine (parent_pid = atoi(argv[6]))
		
	}
	
	printf("Initializing the KOS ...\n");
	
	if (kos_init(&kos)) {
		printf("WARNING Failed to initialize KOS. Exitting ...\n");
		exit(1);
		
	}
	
	printf("Loading the DE ...\n");
	
	char* rom = (char*) 0;
	unsigned long long bytes = 0;
	
	int error_code = load_rom(path, &rom, &bytes);
	if (error_code) {
		printf("ERROR Failed to load ROM (error code = %d), aborting ...\n", error_code);
		return error_code;
		
	}
	
	printf("Creating root machine ...\n");
	root_mid = __create_machine((unsigned long long) path, kos.width, kos.height, 0);
	
	de_program = (program_t*) malloc(sizeof(program_t));
	memset(de_program, 0, sizeof(program_t));
	de_program->pointer = rom;
	
	printf("Starting run setup phase ...\n");
	program_run_setup_phase(de_program);
	
	#if KOS_USES_JNI
		error_code = 0;
	#else
		while (!program_run_loop_phase(de_program));
		error_code = (int) de_program->error_code;
		
		program_free(de_program);
		mfree(rom, bytes);
	#endif
	
	#if !KOS_USES_JNI
		printf("DE return code is %d\n", error_code);
	
		printf("Quitting KOS ...\n");
		kos_quit(&kos);
	#endif
	
	mfree(de_program, sizeof(program_t));
	free_all_machines();
	return error_code;
	
}
