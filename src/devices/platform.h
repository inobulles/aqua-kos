
#ifndef __AQUA__KOS_DEVICES_PLATFORM_H
	#define __AQUA__KOS_DEVICES_PLATFORM_H
	
	#if !KOS_USES_JNI
		#include <pwd.h>
	#endif
	
	static void platform_device_handle(unsigned long long** result, const char* data) {
		if (strcmp(data, "nickname") == 0) {
			#if KOS_USES_JNI
				/// TODO android.os.Build.?
			#else
				register uid_t uid = geteuid();
				register struct passwd* password = getpwuid(uid);
				
				if (password) strncpy(kos_bda_implementation.temp_string, password->pw_name, sizeof(kos_bda_implementation.temp_string));
				else          strcpy (kos_bda_implementation.temp_string, "username");
			#endif
			
			*result = (unsigned long long*) kos_bda_implementation.temp_string;
			
		} else if (strcmp(data, "model") == 0) {
			#if KOS_USES_JNI
				/// TODO android.os.Build.MODEL
			#else
				if (gethostname(kos_bda_implementation.temp_string, sizeof(kos_bda_implementation.temp_string)) == -1) {
					strcpy(kos_bda_implementation.temp_string, "hostname");
					
				}
			#endif
			
			*result = (unsigned long long*) kos_bda_implementation.temp_string;
			
		} else if (strcmp(data, "cpu") == 0) {
			strcpy(kos_bda_implementation.temp_string, "cpu"); /// TODO
			*result = (unsigned long long*) kos_bda_implementation.temp_string;
			
		} else if (strcmp(data, "vendor") == 0) {
			#if KOS_USES_JNI
				/// TODO android.os.Build.BRAND
			#else
				strcpy(kos_bda_implementation.temp_string, "vendor"); /// TODO
			#endif
			
			*result = (unsigned long long*) kos_bda_implementation.temp_string;
			
		} else {
			KOS_DEVICE_COMMAND_WARNING("platform")
			
		}
		
	}
	
#endif
