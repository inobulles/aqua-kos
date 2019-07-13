
#ifndef __AQUA__KOS_DEVICES_PLATFORM_H
	#define __AQUA__KOS_DEVICES_PLATFORM_H
	
	#if !KOS_USES_JNI
		#include <pwd.h>
	#endif
	
	static void platform_device_handle(unsigned long long** result, const char* data) {
		memset(kos_bda_implementation.temp_string, 0, sizeof(kos_bda_implementation.temp_string));
		*result = (unsigned long long*) kos_bda_implementation.temp_string;
		
		if (strcmp(data, "nickname") == 0) {
			#if KOS_USES_JNI
				jstring string = (jstring) CALLBACK(java_platform, callback_env->CallStaticObjectMethod, callback_env->NewStringUTF("nickname"));
				jboolean is_copy = 0;
				strncpy(kos_bda_implementation.temp_string, callback_env->GetStringUTFChars(string, &is_copy), sizeof(kos_bda_implementation.temp_string));
			#else
				register uid_t uid = geteuid();
				register struct passwd* password = getpwuid(uid);
				
				if (password) strncpy(kos_bda_implementation.temp_string, password->pw_name, sizeof(kos_bda_implementation.temp_string));
				else          strcpy (kos_bda_implementation.temp_string, "username");
			#endif
			
		} else if (strcmp(data, "model") == 0) {
			#if KOS_USES_JNI
				jstring string = (jstring) CALLBACK(java_platform, callback_env->CallStaticObjectMethod, callback_env->NewStringUTF("model"));
				jboolean is_copy = 0;
				strncpy(kos_bda_implementation.temp_string, callback_env->GetStringUTFChars(string, &is_copy), sizeof(kos_bda_implementation.temp_string));
			#else
				if (gethostname(kos_bda_implementation.temp_string, sizeof(kos_bda_implementation.temp_string)) == -1) {
					strcpy(kos_bda_implementation.temp_string, "hostname");
					
				}
			#endif
			
		} else if (strcmp(data, "cpu") == 0) {
			strcpy(kos_bda_implementation.temp_string, "cpu"); /// TODO
			
		} else if (strcmp(data, "vendor") == 0) {
			#if KOS_USES_JNI
				jstring string = (jstring) CALLBACK(java_platform, callback_env->CallStaticObjectMethod, callback_env->NewStringUTF("vendor"));
				jboolean is_copy = 0;
				strncpy(kos_bda_implementation.temp_string, callback_env->GetStringUTFChars(string, &is_copy), sizeof(kos_bda_implementation.temp_string));
			#else
				strcpy(kos_bda_implementation.temp_string, "vendor"); /// TODO
			#endif
			
		} else if (strcmp(data, "vm vstring") == 0) {
			unsigned long long zvm_version     (void);
			unsigned long long zvm_stack_bytes (void);
			const char*        zvm_acceleration(void);
			
			sprintf(kos_bda_implementation.temp_string, "ZVM (Zed Virtual Machine) v%lld (0x%llxb stack, %s acceleration)", zvm_version(), zvm_stack_bytes(), zvm_acceleration());
			
		} else if (strcmp(data, "kos info") == 0) {
			#define __STRINGIZE(x) #x // from root.h from lib
			#define   STRINGIZE(x) __STRINGIZE(x)
			
			#if KOS_USES_OPENGL
				#define KOS_INFO_GLIB_STRING "OpenGL " STRINGIZE(KOS_HIGHEST_GL_VERSION_MAJOR) "." STRINGIZE(KOS_HIGHEST_GL_VERSION_MINOR)
				
				#if KOS_USES_OPENGL_DESKTOP
					#define KOS_INFO_GLIB_MORE_STRING "desktop"
				#elif KOS_USES_OPENGLES
					#define KOS_INFO_GLIB_MORE_STRING "ES"
				#endif
			#else
				#define KOS_INFO_GLIB_STRING "no GLIB"
			#endif
			
			#if KOS_USES_JNI
				#define KOS_INFO_INTERFACE "JNI/NDK"
			#elif KOS_USES_SDL2
				#define KOS_INFO_INTERFACE "SDL2"
			#elif KOS_USES_BCM
				#define KOS_INFO_INTERFACE "BCM"
			#else
				#define KOS_INFO_INTERFACE "plain"
			#endif
			
			strcpy(kos_bda_implementation.temp_string, "\"Mega\" (universal) KOS using " KOS_INFO_GLIB_STRING " (" KOS_INFO_GLIB_MORE_STRING ") with the " KOS_INFO_INTERFACE " interface");
			
		} else {
			KOS_DEVICE_COMMAND_WARNING("platform")
			
		}
		
	}
	
#endif
