
#include <unistd.h>

#include "src/kos.h"
#include "asm/asm.h"

void mfree(void* ptr, unsigned long long bytes) { // for some reason, this was not already defined
	free(ptr);
	
}

static kos_t kos;

#define ROM_PATH "ROM.zed"

static unsigned long long kos_roms_loaded = 0;
static program_t* current_de_program = nullptr;

static signed long long __load_rom(unsigned long long __path) {
	const char* ____path;
	
	if (kos_roms_loaded++) {
		GET_PATH((char*) __path);
		____path = path;
		
	} else {
		____path = (const char*) __path;
		
	}
	
	const char* path = ____path;
	
	void*                 __pointer_current_program_previous = __pointer_current_program;
	__pointer__program_t* __pointer___this_previous          = __pointer___this;
	
	#if KOS_USES_JNI
		program_t* de_program = (program_t*) malloc(sizeof(program_t));
	#else
		program_t __de_program;
		program_t* de_program = &__de_program;
	#endif
	
	current_de_program = de_program;
	
	char*              rom   = nullptr;
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
		
		__pointer_current_program = __pointer_current_program_previous;
		__pointer___this          = __pointer___this_previous;
		
		return de_program->error_code;
	#endif
	
}

static const char* a_out_execution_command = "./a.out";

signed long long load_rom(unsigned long long path) {
	char command_buffer[4096];
	sprintf(command_buffer, "%s root/%s", a_out_execution_command, (const char*) path);
	return system(command_buffer);
	
}

int main(int argc, char** argv) {
	printf("\nControl passed to the CW\n");
	printf("Initializing the KOS ...\n");
	
	if (kos_init(&kos)) {
		printf("WARNING Failed to initialize KOS. Exitting ...\n");
		exit(1);
		
	}
	
	printf("Entering the DE ...\n");
	char* path;
	
	a_out_execution_command = argv[0];
	
	if (argc <= 1) path = (char*) ROM_PATH;
	else           path = argv[1];
	
	int error_code = (int) __load_rom((unsigned long long) path);

	#if !KOS_USES_JNI
		printf("DE return code is %d\n", error_code);
	
		printf("Quitting KOS ...\n");
		kos_quit(&kos);
	#endif
	
	return error_code;
	
}
