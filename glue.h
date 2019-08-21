
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

void mfree(void* ptr, unsigned long long bytes) {
	free(ptr);
	
}

#include "src/kos.h"

static char* first_argv;
#include "src/functions/machine.h"

#include "zvm/zvm.h"

static kos_t kos;
#define ROM_PATH "rom.zed"
static zvm_program_t* de_program;

static int load_rom(const char* path, char** rom, unsigned long long* bytes) {
	#if KOS_USES_JNI
		if (load_asset_bytes(path, rom, bytes)) {
			if (!default_assets) {
				ALOGW("WARNING Could not load the ROM from internal / external storage. Trying from assets ...\n");
				default_assets = true;
				
				if (load_asset_bytes(path, rom, bytes)) {
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

void main_free(void) {
	mfree(de_program, sizeof(zvm_program_t));
	free_all_machines();
	
}

int main(int argc, char** argv) {
	first_argv = argv[0];
	
	printf("Parsing arguments ...\n");
	char* path;
	
	if (argc <= 1) path = (char*) ROM_PATH;
	else           path = argv[1];
	
	kos.text_only = 0;
	
	kos.width  = KOS_ORIGINAL_WIDTH;
	kos.height = KOS_ORIGINAL_HEIGHT;
	
	if (argc > 2 && argv[2][0] == 'x') {
		printf("Text only machine\n");
		kos.text_only = 1;
		
	} if (argc > 4) {
		printf("Video resolution %s x %s\n", argv[3], argv[4]);
		
		kos.width  = atoi(argv[3]);
		kos.height = atoi(argv[4]);
		
	} if (argc > 6) {
		printf("Child machine (parent PID = %s, MID = %s)\n", argv[5], argv[6]);
		root_mid = (unsigned long long) atoi(argv[6]);
		/// TODO is child machine (parent_pid = atoi(argv[5]))
		
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
	root_mid = !root_mid ? __create_machine((unsigned long long) path, (unsigned long long) kos.width, (unsigned long long) kos.height, 0) : root_mid;
	
	de_program = (zvm_program_t*) malloc(sizeof(zvm_program_t));
	memset(de_program, 0, sizeof(zvm_program_t));
	de_program->pointer = rom;
	
	printf("Starting run setup phase ...\n");
	zvm_program_run_setup_phase(de_program);
	
	#if KOS_USES_JNI
		error_code = 0;
	#else
		while (!zvm_program_run_loop_phase(de_program));
		error_code = de_program->error_code;
		
		//~ program_free(de_program);
		mfree(rom, bytes);
	#endif
	
	#if !KOS_USES_JNI
		printf("DE return code is %d\n", error_code);
	
		printf("Quitting KOS ...\n");
		kos_quit(&kos);
	
		main_free();
	#endif
	
	return error_code;
	
}
