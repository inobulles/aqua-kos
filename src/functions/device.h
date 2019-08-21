
#ifndef __AQUA__KOS_FUNCTION_DEVICE_H
	#define __AQUA__KOS_FUNCTION_DEVICE_H

	#define DEVICE_NULL            0
	#define DEVICE_TEXTURE         1
	#define DEVICE_KEYBOARD        2
	#define DEVICE_WM              3
	#define DEVICE_MATH            4
	#define DEVICE_CLOCK           5
	#define DEVICE_ANDROID         6
	#define DEVICE_FRAMEBUFFER     7
	#define DEVICE_SHADER          8
	#define DEVICE_REQUESTS        9
	#define DEVICE_DISCORD         10
	#define DEVICE_GL              11
	#define DEVICE_GL_BATCH        12
	#define DEVICE_FS              13
	#define DEVICE_KEYBOARD_DIALOG 14
	#define DEVICE_DEBUG           15
	#define DEVICE_JOYSTICK        16
	#define DEVICE_DDS             17
	#define DEVICE_SURFACE         18
	#define DEVICE_FONT            19
	#define DEVICE_MOUSE           20
	#define DEVICE_SOCKET          21
	#define DEVICE_PREDEFINED      22
	#define DEVICE_BMP             23
	#define DEVICE_KTX             24
	#define DEVICE_PLATFORM        25
	#define DEVICE_SOUND           26
	#define DEVICE_MP3             27
	#define DEVICE_JPG             28
	#define DEVICE_PNG             29
	#define DEVICE_CLIPBOARD       30
	
	// compute devices
	
	#define COMPUTE_BASE 0xC0C0
	
	#define __COMPUTE_COMPILER_DEVICE 0
	#define __COMPUTE_EXECUTOR_DEVICE 1
	
	#define COMPUTE_COMPILER_DEVICE(x) (COMPUTE_BASE + __COMPUTE_COMPILER_DEVICE + (x) * 2)
	#define COMPUTE_EXECUTOR_DEVICE(x) (COMPUTE_BASE + __COMPUTE_EXECUTOR_DEVICE + (x) * 2)
	
	#define DEVICE_COMPUTE_CUDA_COMPILER (COMPUTE_COMPILER_DEVICE(0))
	#define DEVICE_COMPUTE_CUDA_EXECUTOR (COMPUTE_EXECUTOR_DEVICE(0))
	
	#define KOS_DEVICE_COMMAND_WARNING(device_name) printf("WARNING The command you have passed to the " device_name " device (%s) is unrecognized\n", (const char*) data);
	
	unsigned long long kos_create_device(unsigned long long __device) {
		const char* device = (const char*) __device;
		
		if      (strcmp(device, "texture")     == 0) return DEVICE_TEXTURE;
		else if (strcmp(device, "wm")          == 0) return DEVICE_WM;
		else if (strcmp(device, "math")        == 0) return DEVICE_MATH;
		else if (strcmp(device, "clock")       == 0) return DEVICE_CLOCK;
		else if (strcmp(device, "framebuffer") == 0) return DEVICE_FRAMEBUFFER;
		else if (strcmp(device, "shader")      == 0) return DEVICE_SHADER;
		else if (strcmp(device, "gl")          == 0) return DEVICE_GL;
		else if (strcmp(device, "gl batch")    == 0) return DEVICE_GL_BATCH;
		else if (strcmp(device, "fs")          == 0) return DEVICE_FS;
		else if (strcmp(device, "debug")       == 0) return DEVICE_DEBUG;
		else if (strcmp(device, "surface")     == 0) return DEVICE_SURFACE;
		else if (strcmp(device, "font")        == 0) return DEVICE_FONT;
		else if (strcmp(device, "mouse")       == 0) return DEVICE_MOUSE;
		else if (strcmp(device, "socket")      == 0) return DEVICE_SOCKET;
		else if (strcmp(device, "predefined")  == 0) return DEVICE_PREDEFINED;
		else if (strcmp(device, "bmp")         == 0) return DEVICE_BMP;
		else if (strcmp(device, "joystick")    == 0) return DEVICE_JOYSTICK;
		else if (strcmp(device, "ktx")         == 0) return DEVICE_KTX;
		else if (strcmp(device, "platform")    == 0) return DEVICE_PLATFORM;
		else if (strcmp(device, "keyboard")    == 0) return DEVICE_KEYBOARD; /// TODO Add keyboard support for Android
		else if (strcmp(device, "jpg")         == 0) return DEVICE_JPG;
		else if (strcmp(device, "png")         == 0) return DEVICE_PNG;
		else if (strcmp(device, "clipboard")   == 0) return DEVICE_CLIPBOARD;
		
		#if KOS_USES_JNI // JNI specific
			else if (strcmp(device, "android")  == 0) return DEVICE_ANDROID;
		#else // absolutely not JNI
		#endif
		#if KOS_USES_OPENGL_DESKTOP
			else if (strcmp(device, "dds") == 0) return DEVICE_DDS; /// TODO Add ETC compression for android
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
		#ifdef __HAS_AUDIO
			else if (strcmp(device, "sound")    == 0) return DEVICE_SOUND;
			else if (strcmp(device, "mp3")      == 0) return DEVICE_MP3;
		#endif
		
		else return DEVICE_NULL;
		
	}
	
	typedef struct {
		uint64_t hour;
		uint64_t minute;
		uint64_t second;
		
		uint64_t day;
		uint64_t month;
		uint64_t year;
		
		uint64_t week_day;
		uint64_t year_day;
		
	} time_device_t;
	
	typedef struct {
		unsigned long long temp_value;
		char temp_string[4096];
		unsigned long long temp_value_field[8];
		
	} kos_bda_extension_t;
	
	#define KOS_BDA_EXTENSION
	kos_bda_extension_t kos_bda_implementation;
	
	#include "../devices/clock.h"
	#include "../devices/math.h"
	#include "../devices/keyboard.h"
	#include "../devices/joystick.h"
	#include "../devices/compute/cuda.h"
	#include "../devices/fs.h"
	#include "../devices/framebuffer.h"
	#include "../devices/shader.h"
	#include "../devices/surface.h"
	#include "../devices/gl_batch.h"
	#include "../devices/gl.h"
	#include "../devices/texture.h"
	#include "../devices/wm.h"
	#include "../devices/debug.h"
	#include "../devices/font.h"
	#include "../devices/mouse.h"
	#include "../devices/socket.h"
	#include "../devices/bmp.h"
	#include "../devices/predefined.h"
	#include "../devices/ktx.h"
	#include "../devices/platform.h"
	#include "../devices/jpg.h"
	#include "../devices/png.h"
	#include "../devices/clipboard.h"
	
	#if KOS_USES_OPENGL_DESKTOP
		#include "../devices/dds.h"
	#endif
	#ifdef __HAS_CURL
		#include "../devices/requests.h"
	#endif
	#ifdef __HAS_DISCORD
		#include "../devices/discord.h"
	#endif
	#ifdef __HAS_AUDIO
		#include "../devices/sound.h"
		#include "../devices/mp3.h"
	#endif
	#if KOS_USES_JNI
		#include "../devices/android.h"
	#endif
	
	unsigned long long* kos_send_device(unsigned long long device, unsigned long long __data) {
		const char* data = (const char*) __data;
		
		kos_bda_implementation.temp_value = 0;
		unsigned long long* result = &kos_bda_implementation.temp_value;
		
		switch (device) {
			case DEVICE_CLOCK:                        clock_device_handle(&result, data); break;
			case DEVICE_MATH:                          math_device_handle(&result, data); break;
			case DEVICE_KEYBOARD:                  keyboard_device_handle(&result, data); break;
			case DEVICE_JOYSTICK:                  joystick_device_handle(&result, data); break;
			case DEVICE_COMPUTE_CUDA_COMPILER: cuda_compile_device_handle(&result, data); break;
			case DEVICE_COMPUTE_CUDA_EXECUTOR: cuda_execute_device_handle(&result, data); break;
			case DEVICE_FS:                              fs_device_handle(&result, data); break;
			case DEVICE_FRAMEBUFFER:            framebuffer_device_handle(&result, data); break;
			case DEVICE_SHADER:                      shader_device_handle(&result, data); break;
			case DEVICE_GL_BATCH:                  gl_batch_device_handle(&result, data); break;
			case DEVICE_GL:                              gl_device_handle(&result, data); break;
			case DEVICE_TEXTURE:                    texture_device_handle(&result, data); break;
			case DEVICE_WM:                              wm_device_handle(&result, data); break;
			case DEVICE_DEBUG:                        debug_device_handle(&result, data); break;
			case DEVICE_FONT:                          font_device_handle(&result, data); break;
			case DEVICE_SURFACE:                    surface_device_handle(&result, data); break;
			case DEVICE_PREDEFINED:              predefined_device_handle(&result, data); break;
			case DEVICE_BMP:                            bmp_device_handle(&result, data); break;
			case DEVICE_SOCKET:                      socket_device_handle(&result, data); break;
			case DEVICE_KTX:                            ktx_device_handle(&result, data); break;
			case DEVICE_MOUSE:                        mouse_device_handle(&result, data); break;
			case DEVICE_PLATFORM:                  platform_device_handle(&result, data); break;
			case DEVICE_JPG:                            jpg_device_handle(&result, data); break;
			case DEVICE_PNG:                            png_device_handle(&result, data); break;
			case DEVICE_CLIPBOARD:                clipboard_device_handle(&result, data); break;
			
			#if KOS_USES_OPENGL_DESKTOP
				case DEVICE_DDS: dds_device_handle(&result, data); break;
			#endif
			#ifdef __HAS_CURL
				case DEVICE_REQUESTS: requests_device_handle(&result, data); break;
			#endif
			#ifdef __HAS_DISCORD
				case DEVICE_DISCORD: discord_device_handle(&result, data); break;
			#endif
			#ifdef __HAS_AUDIO
				case DEVICE_SOUND: sound_device_handle(&result, data); break;
				case DEVICE_MP3:     mp3_device_handle(&result, data); break;
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
