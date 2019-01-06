
#include <unistd.h>
#include "src/lib/animation.h"

#ifndef     LOAD_PROGRAM_SUPPORTED
	#define LOAD_PROGRAM_SUPPORTED 1
	
	void load_program(unsigned long long rom_data, unsigned long long rom_bytes, unsigned long long in_animation_speed, unsigned long long out_animation_speed, unsigned long long width, unsigned long long height);
	static unsigned char current_video_flip_is_root_window = 1;
	
	static int          load_program_overlay = 0;
	static int          load_program_overlay_dimensions[2];
	
	static unsigned int load_program_overlay_texture;
	static unsigned int load_program_overlay_framebuffer;
	
	static animation_t  load_program_overlay_animation;
	static unsigned int load_program_overlay_stage = 3;
#endif

static void free_load_program_overlay_last(void);

#include "src/kos.h"
#include "asm/asm.h"

#if LOAD_PROGRAM_SUPPORTED
	static uint64_t* load_program_overlay_data;
	static int       load_program_overlay_bpp;
	
	static program_t load_program_overlay_de_program;
	
	static void free_load_program_overlay_last(void) {
		if (load_program_overlay) {
			load_program_overlay       = 0;
			load_program_overlay_stage = 3;
			
			framebuffer_remove(load_program_overlay_framebuffer);
			texture_remove    (load_program_overlay_texture);
			free              (load_program_overlay_data);
			
		}
		
	}
	
	static void free_load_program_overlay_first(void) {
		if (load_program_overlay) {
			program_free(&load_program_overlay_de_program);
			
		}
		
	}
	
	static void free_load_program_overlay(void) {
		free_load_program_overlay_first();
		free_load_program_overlay_last ();
		
	}
#endif

void mfree(void* ptr, unsigned long long bytes) { // for some reason, this was not already defined
	free(ptr);
	
}

static kos_t kos;

#define ROM_PATH "ROM.zed"

static unsigned long long kos_roms_loaded = 0;
static program_t* current_de_program = (program_t*) 0;

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
	#endif
	
	de_program->pointer = rom;

	printf("Starting run setup phase ...\n");
	program_run_setup_phase(de_program);

	#if KOS_USES_JNI
		return 0;
	#else
		while (1) {
			#if LOAD_PROGRAM_SUPPORTED
				if (load_program_overlay_stage < 2) { // loop the overlay program
					current_video_flip_is_root_window = 0;
					framebuffer_bind(load_program_overlay_framebuffer, 0, 0, load_program_overlay_dimensions[0], load_program_overlay_dimensions[1]);
					
					while (!program_run_loop_phase(&load_program_overlay_de_program)) {
						if (video_flip_called) {
							break;
							
						}
						
					}
					
					if (!video_flip_called) {
						free_load_program_overlay_first();
						load_program_overlay_stage++;
						
					} else {
						video_flip_called = 0;
						
					}
					
					current_video_flip_is_root_window = 1;
					framebuffer_bind(0, 0, 0, video_width(), video_height());
					
				} while (!
			#else
				if (
			#endif
			
			program_run_loop_phase(de_program)) { // loop the root program
				#if LOAD_PROGRAM_SUPPORTED
					if (video_flip_called) {
				#endif
				
				break;
				
				#if LOAD_PROGRAM_SUPPORTED
					}
				#endif
				
			}
			
			#if LOAD_PROGRAM_SUPPORTED
				if (!video_flip_called && !load_program_overlay) {
					break;
					
				} else {
					video_flip_called = 0;
					
				}
			#endif
			
		}
		
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

void load_program(unsigned long long rom_data, unsigned long long rom_bytes, unsigned long long in_animation_speed, unsigned long long out_animation_speed, unsigned long long width, unsigned long long height) {
	#if LOAD_PROGRAM_SUPPORTED
		free_load_program_overlay();
		load_program_overlay = 1;
		
		load_program_overlay_dimensions[0] = width;
		load_program_overlay_dimensions[1] = height;
		
		load_program_overlay_bpp     = 24; /// TODO argument to make this 32 for transparent windows
		load_program_overlay_data    = (uint64_t*) malloc                        ((load_program_overlay_bpp >> 3) * load_program_overlay_dimensions[0] * load_program_overlay_dimensions[1]);
		load_program_overlay_texture = __texture_create(load_program_overlay_data, load_program_overlay_bpp,        load_program_overlay_dimensions[0],  load_program_overlay_dimensions[1], 0);
		
		load_program_overlay_framebuffer = framebuffer_create(load_program_overlay_texture);
		
		load_program_overlay_de_program.pointer = (void*) rom_data;
		program_run_setup_phase(&load_program_overlay_de_program);
		
		new_animation(&load_program_overlay_animation, 0.0f, (float) in_animation_speed / FLOAT_ONE);
		load_program_overlay_stage = 0;
	#else
		printf("WARNING This platform does not support the load_program function (USES_LOAD_PROGRAM_OVERLAYS = %d)\n", USES_LOAD_PROGRAM_OVERLAYS);
	#endif
	
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
	free_load_program_overlay();
	
	#if !KOS_USES_JNI
		printf("DE return code is %d\n", error_code);
	
		printf("Quitting KOS ...\n");
		kos_quit(&kos);
	#endif
	
	return error_code;
	
}
