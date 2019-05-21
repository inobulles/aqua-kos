
#ifndef __AQUA__KOS_DEVICES_ANDROID_H
	#define __AQUA__KOS_DEVICES_ANDROID_H
	
	static unsigned char text_input_has_response = 0;
	
	static void android_device_handle(unsigned long long** result, const char* data) {
		if (strcmp(data, "keyboard dialog") == 0) {
			CALLBACK_VOID_NO_PARAMS(java_open_text_input);
			text_input_has_response = 0;
			
			extern bool cw_pause;
			cw_pause = true;
			
		} else if (strncmp(data, "package exists ", 13) == 0) {
			kos_bda_implementation.temp_value = CALLBACK_INT(java_package_exists, callback_env->NewStringUTF(data + 13));
			*result = (unsigned long long*) &kos_bda_implementation.temp_value;
			
		} else {
			KOS_DEVICE_COMMAND_WARNING("android")
			
		}
		
	}
	
#endif
