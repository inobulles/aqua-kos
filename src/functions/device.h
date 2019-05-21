
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
			else if (strcmp(device, "android")  == 0) return DEVICE_ANDROID;
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
			else if (strcmp(device, "discord")  == 0) return DEVICE_DISCORD;
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
	#include "../devices/texture.h"
	#include "../devices/wm.h"
	#include "../devices/debug.h"
	
	#ifdef __HAS_CURL
		#include "../devices/requests.h"
	#endif
	#ifdef __HAS_DISCORD
		#include "../devices/discord.h"
	#endif
	#if KOS_USES_JNI
		#include "../devices/android.h"
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
			case DEVICE_TEXTURE:                    texture_device_handle(&result, data); break;
			case DEVICE_WM:                              wm_device_handle(&result, data); break;
			case DEVICE_DEBUG:                        debug_device_handle(&result, data); break;
			
			#ifdef __HAS_CURL
				case DEVICE_REQUESTS: requests_device_handle(&result, data); break;
			#endif
			#ifdef __HAS_DISCORD
				case DEVICE_DISCORD: discord_device_handle(&result, data); break;
			#endif
			#if KOS_USES_JNI
				case DEVICE_ANDROID: android_device_handle(&result, data); break;
			#endif
			
			case DEVICE_NULL: {
				printf("WARNING The device you have selected is DEVICE_NULL\n");
				break;
				
			} default: {
				printf("WARNING Device %lld does not seem to exist or doesn't accept `get` commands\n", device);
				break;
				
			}
			
		}
		
		return result;
		
	}
	
#endif
