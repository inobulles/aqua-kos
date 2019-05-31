
#ifndef __AQUA__SDL2_SRC_FUNCTIONS_SYSTEM_H
	#define __AQUA__SDL2_SRC_FUNCTIONS_SYSTEM_H
	
	#define PLATFORM_64BIT 0b1
	#define PLATFORM_UNIX 0b10
	#define PLATFORM_WINDOWS 0b100
	#define PLATFORM_CANCER 0b1000
	#define PLATFORM_MOBILE 0b10000
	#define PLATFORM_CUSTOM 0b100000
	#define PLATFORM_KNOWN 0b1000000
	
	#define PLATFORM_OTHER PLATFORM_64BIT
	#define PLATFORM_LINUX PLATFORM_KNOWN | PLATFORM_64BIT | PLATFORM_UNIX
	#define PLATFORM_WIN64 PLATFORM_KNOWN | PLATFORM_64BIT | PLATFORM_WINDOWS
	#define PLATFORM_WIN32 PLATFORM_KNOWN | PLATFORM_WINDOWS
	#define PLATFORM_MACOS PLATFORM_KNOWN | PLATFORM_64BIT | PLATFORM_UNIX | PLATFORM_CANCER
	#define PLATFORM_IOS PLATFORM_KNOWN | PLATFORM_64BIT | PLATFORM_UNIX | PLATFORM_CANCER | PLATFORM_MOBILE
	#define PLATFORM_ANDROID PLATFORM_KNOWN | PLATFORM_64BIT | PLATFORM_UNIX | PLATFORM_MOBILE
	#define PLATFORM_AQUA PLATFORM_KNOWN | PLATFORM_CUSTOM
	
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
	
	#include "device.h"
	#include "native.h"
	
#endif
