
#ifndef __AQUA__SDL2_SRC_FUNCTIONS_SYSTEM_H
	#define __AQUA__SDL2_SRC_FUNCTIONS_SYSTEM_H
	
	#include "../macros_and_inclusions.h"
	
	unsigned long long get_platform(void) {
		#if KOS_USES_JNI
			return PLATFORM_ANDROID;
		#else
			#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
				return PLATFORM_WIN32;
			#elif defined(_WIN64)
				return PLATFORM_WIN64;
			#elif defined(__CYGWIN__) && !defined(_WIN32)
				return PLATFORM_UNIX;
			#elif defined(__APPLE__) && defined(__MACH__)
				printf("WARNING Apple based platform detected, consider changing your OS\n");

				#if TARGET_IPHONE_SIMULATOR == 1 || TARGET_OS_IPHONE == 1
					return PLATFORM_IOS;
				#elif TARGET_OS_MAC == 1
					return PLATFORM_MACOS;
				#else
					return PLATFORM_CANCER;
				#endif
			#elif defined(BSD)
				printf("WARNING BSD based system detected. Returning Unix, as BSD is not included in the AQUA system standard\n");
				return PLATFORM_UNIX;
			#elif defined(__linux__)
				return PLATFORM_LINUX;
			#elif defined(__unix__)
				return PLATFORM_UNIX;
			#else
				return PLATFORM_OTHER;
			#endif
		#endif
		
	}
	
	unsigned long long platform_system(unsigned long long __command) {
		const char* command = (const char*) __command;
		
		#if SYSTEM_ACCESS
			return (unsigned long long) system(command);
		#else
			printf("WARNING You do not seem to have the privileges (SYSTEM_ACCESS = %d) to run the command `%s`\n", SYSTEM_ACCESS, command);
			return 1;
		#endif
		
	}
	
	void platform(void) {
		KOS_TODO
		
	}
	
	#include "device.h"
	
#endif
