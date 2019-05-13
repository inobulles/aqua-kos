
#include <unistd.h>

#include "src/kos.h"
#include "asm/asm.h"

void mfree(void* ptr, unsigned long long bytes) { // for some reason, this was not already defined
	free(ptr);
	
}

static kos_t kos;
#define ROM_PATH "ROM.zed" /// TODO make this rom.zed

static program_t __de_program;
static program_t*  de_program;

static int load_rom(const char* path) {
	de_program = (program_t*) &__de_program;
	memset(de_program, 0, sizeof(program_t));
	
	char*              rom   = (char*) 0;
	unsigned long long bytes = 0;
	
	#if KOS_USES_JNI
		if (load_asset_bytes((const char*) __path, &rom, &bytes)) {
			if (!default_assets) {
				ALOGW("WARNING Could not load the ROM from internal / external storage. Trying from assets ...\n");
				default_assets = true;
				
				if (load_asset_bytes((const char*) __path, &rom, &bytes)) {
					ALOGE("ERROR Could not load ROM from assets either\n");
					
				}
				
			} else {
				ALOGE("ERROR Could not load the ROM\n");
				
			}
			
		}
	#else
		FILE* fp = fopen(path, "rb");
		
		if (!fp) {
			printf("WARNING Could not open ROM file (%s)\n", path);
			kos_quit(&kos);
			exit(1);
	
		}
	
		fseek(fp, 0, SEEK_END);
		bytes = (unsigned long long) ftell(fp);
		rewind(fp);
	
		rom = (char*) malloc(bytes);
		fread(rom, sizeof(char), bytes, fp);
		fclose(fp);
	#endif
	
	de_program->pointer = rom;

	printf("Starting run setup phase ...\n");
	program_run_setup_phase(de_program);

	#if KOS_USES_JNI
		return 0;
	#else
		while (!program_run_loop_phase(de_program));
		
		program_free(de_program);
		mfree(rom, bytes);
		
		return (int) de_program->error_code;
	#endif
	
}


int main(int argc, char** argv) {
	printf("Initializing the KOS ...\n");
	
	if (kos_init(&kos)) {
		printf("WARNING Failed to initialize KOS. Exitting ...\n");
		exit(1);
		
	}
	
	printf("Entering the DE ...\n");
	char* path;
	
	if (argc <= 1) path = (char*) ROM_PATH;
	else           path = argv[1];
	
	int error_code = load_rom(path);
	
	#if !KOS_USES_JNI
		printf("DE return code is %d\n", error_code);
	
		printf("Quitting KOS ...\n");
		kos_quit(&kos);
	#endif
	
	return error_code;
	
}
