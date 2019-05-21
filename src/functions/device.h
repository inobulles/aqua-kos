
#ifndef __AQUA__KOS_FUNCTION_DEVICE_H
	#define __AQUA__KOS_FUNCTION_DEVICE_H
	
	#define KOS_DEVICE_COMMAND_WARNING(device_name) printf("WARNING The command you have passed to the " device_name " device (%s) is unrecognized\n", (const char*) data);
	
	unsigned long long is_device_supported(unsigned long long __device) {
		const char* device = (const char*) __device;
		
		if      (strcmp(device, "texture")  == 0) return DEVICE_TEXTURE;
		else if (strcmp(device, "wm")       == 0) return DEVICE_WM;
		else if (strcmp(device, "math")     == 0) return DEVICE_MATH;
		else if (strcmp(device, "clock")    == 0) return DEVICE_CLOCK;
		else if (strcmp(device, "fbo")      == 0) return DEVICE_FBO;
		else if (strcmp(device, "shader")   == 0) return DEVICE_SHADER;
		else if (strcmp(device, "gl")       == 0) return DEVICE_GL;
		else if (strcmp(device, "gl batch") == 0) return DEVICE_GL_BATCH;
		else if (strcmp(device, "fs")       == 0) return DEVICE_FS;
		else if (strcmp(device, "debug")    == 0) return DEVICE_DEBUG;

		#if KOS_USES_JNI // JNI specific
			else if (strcmp(device, "android")         == 0) return DEVICE_ANDROID;
			else if (strcmp(device, "keyboard dialog") == 0) return DEVICE_KEYBOARD_DIALOG;
		#else // absolutely not JNI
			else if (strcmp(device, "keyboard") == 0) return DEVICE_KEYBOARD; /// TODO Add keyboard support for Android
			else if (strcmp(device, "joystick") == 0) return DEVICE_JOYSTICK; /// TODO Add joystick support for Android
		#endif
		
		// compute
		
		else if (strcmp(device, "nvcc") == 0 && !system("command -v nvcc")) return DEVICE_COMPUTE_CUDA_COMPILER;
		else if (strcmp(device, "cuda") == 0)                               return DEVICE_COMPUTE_CUDA_EXECUTOR;
		
		// extensions
		
		#ifdef __HAS_CURL
			else if (strcmp(device, "requests") == 0) return DEVICE_REQUESTS;
		#endif
		#ifdef __HAS_DISCORD
			else if (strcmp(device, "discord") == 0) return DEVICE_DISCORD;
		#endif
		
		else return DEVICE_NULL;
		
	}
	
	typedef struct {
		unsigned long long temp_value;
		char temp_string[4096];
		time_device_t temp_time_device;
		
	} kos_bda_extension_t;

	#define KOS_BDA_EXTENSION
	kos_bda_extension_t kos_bda_implementation;
	
	#include "../devices/clock.h"
	#include "../devices/math.h"
	#include "../devices/keyboard.h"
	#include "../devices/joystick.h"
	#include "../devices/compute/cuda.h"
	#include "../devices/fs.h"
	#include "../devices/fbo.h"
	#include "../devices/shader.h"
	#include "../devices/gl_batch.h"
	#include "../devices/gl.h"
	
	#ifdef __HAS_CURL
		#include "requests.h"
	#endif
	#ifdef __HAS_DISCORD
		#include "discord.h"
	#endif
	
	#if KOS_USES_JNI
		static unsigned char text_input_has_response = 0;
	#endif
	
	unsigned long long* get_device(unsigned long long device, unsigned long long __data) {
		const         char* data   =         (const char*) __data;
		unsigned long long* result = (unsigned long long*) 0;
		
		switch (device) {
			case DEVICE_CLOCK:                        clock_device_handle(&result, data); break;
			case DEVICE_MATH:                          math_device_handle(&result, data); break;
			case DEVICE_KEYBOARD:                  keyboard_device_handle(&result, data); break;
			case DEVICE_JOYSTICK:                  joystick_device_handle(&result, data); break;
			case DEVICE_COMPUTE_CUDA_COMPILER: cuda_compile_device_handle(&result, data); break;
			case DEVICE_COMPUTE_CUDA_EXECUTOR: cuda_execute_device_handle(&result, data); break;
			case DEVICE_FS:                              fs_device_handle(&result, data); break;
			case DEVICE_FBO:                            fbo_device_handle(&result, data); break;
			case DEVICE_SHADER:                      shader_device_handle(&result, data); break;
			case DEVICE_GL_BATCH:                  gl_batch_device_handle(&result, data); break;
			case DEVICE_GL:                              gl_device_handle(&result, data); break;
			
			#if KOS_USES_JNI
				case DEVICE_ANDROID: {
					if (extra[0] == 'p' && extra[1] == 'k' && extra[2] == 'g' && extra[3] == 'e') {
						extra += 4;
						kos_bda_implementation.previous_package_existence = (unsigned long long) CALLBACK_INT(java_package_exists, callback_env->NewStringUTF(extra));
						result = &kos_bda_implementation.previous_package_existence;

					} else {
						KOS_DEVICE_COMMAND_WARNING("android")

					}

					break;

				}
			#endif

			case DEVICE_KEYBOARD_DIALOG: {
				#if KOS_USES_JNI
					if (strcmp(extra, "open") == 0) {
						CALLBACK_VOID_NO_PARAMS(java_open_text_input);
						text_input_has_response = 0;

						extern bool cw_pause;
						cw_pause = true;

					} else
				#endif

				{ KOS_DEVICE_COMMAND_WARNING("keyboard dialog") }
				break;

			} case DEVICE_NULL: {
				printf("WARNING The device you have selected is DEVICE_NULL\n");
				break;
				
			} default: {
				printf("WARNING Device %lld does not seem to exist or doesn't accept `get` commands\n", device);
				break;
				
			}
			
		}
		
		return result;
		
	}
	
	#ifdef __HAS_CURL
		typedef struct {
			kos_request_response_t request_response;
			unsigned long long     pointer_to_const_url;
			
		} request_device_struct_t;
	#endif
	#ifdef __HAS_DISCORD
		typedef struct {
			kos_discord_rpc_t discord_rpc;
			
		} discord_device_struct_t;
	#endif
	
	void heap_memory_snapshot_start(void);
	void heap_memory_snapshot_end  (void);
	
	void send_device(unsigned long long device, unsigned long long __extra, unsigned long long __data) {
		const char*         extra = (const char*)        __extra;
		unsigned long long* data  = (unsigned long long*) __data;
		
		switch (device) {
			case DEVICE_TEXTURE: {
				if (strcmp(extra, "sharp") == 0) SHARP_TEXTURES = *data;
				else KOS_DEVICE_COMMAND_WARNING("texture")
				
				break;
				
			} case DEVICE_WM: {
				if (strcmp(extra, "visible") == 0) {
					if (*data == HIDDEN) {
						#if KOS_USES_SDL2
							SDL_MinimizeWindow(current_kos->window);
						#endif
						
					}
					
				} else {
					KOS_DEVICE_COMMAND_WARNING("wm")
					
				}
				
				break;
				
			} case DEVICE_JOYSTICK: {
				if (strcmp(extra, "select") == 0) {
					kos_select_joystick(*data);
					
				} else {
					KOS_DEVICE_COMMAND_WARNING("joystick")
					
				}
				
				break;
				
			} case DEVICE_DEBUG: {
				if (strcmp(extra, "mem.snap.start") == 0) {
					printf("=== debug.mem.snap.start ===\n");
					heap_memory_snapshot_start();
					
				} else if (strcmp(extra, "mem.snap.end") == 0) {
					printf("=== debug.mem.snap.end ===\n");
					heap_memory_snapshot_end();
					
				} else {
					KOS_DEVICE_COMMAND_WARNING("debug")
					
				}
				
				break;
				
			}
			#ifdef __HAS_CURL
				case DEVICE_REQUESTS: {
					request_device_struct_t* request_device_struct = (request_device_struct_t*) data;
					
					if      (strcmp(extra, "get")  == 0) kos_requests_get (&request_device_struct->request_response, (const char*) request_device_struct->pointer_to_const_url);
					else if (strcmp(extra, "free") == 0) kos_requests_free(&request_device_struct->request_response);
					else KOS_DEVICE_COMMAND_WARNING("requests")
					
					break;
					
				}
			#endif
			#ifdef __HAS_DISCORD
				case DEVICE_DISCORD: {
					discord_device_struct_t* discord_device_struct = (discord_device_struct_t*) data;
					
					if      (strcmp(extra, "rpc init")    == 0) init_discord_rpc  ((unsigned long long) data);
					else if (strcmp(extra, "rpc loop")    == 0) loop_discord_rpc  (&discord_device_struct->discord_rpc);
					else if (strcmp(extra, "rpc update")  == 0) update_discord_rpc(&discord_device_struct->discord_rpc);
					else if (strcmp(extra, "rpc dispose") == 0) dispose_discord_rpc();
					else KOS_DEVICE_COMMAND_WARNING("requests")
					
					break;
					
				}
			#endif
			case DEVICE_NULL: {
				printf("WARNING The device you have selected is DEVICE_NULL\n");
				break;
				
			} default: {
				printf("WARNING Device %lld does not seem to exist or doesn't accept `send` commands\n", device);
				break;
				
			}
			
		}
		
	}
	
#endif
