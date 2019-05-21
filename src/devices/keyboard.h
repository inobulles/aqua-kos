
#ifndef __AQUA__KOS_DEVICES_KEYBOARD_H
	#define __AQUA__KOS_DEVICES_KEYBOARD_H
	
	unsigned long long get_device_keyboard_key     = 0;
	unsigned long long get_device_keyboard_keycode = 0;
	
	static void keyboard_device_handle(unsigned long long** result, const char* data) {
		if (strcmp(data, "count") == 0) {
			kos_bda_implementation.temp_value = 1; /// TODO Add support for multiple keyboards (+ select command to select what keyboard to interact with)
			
		} else if (strcmp(data, "press scancode") == 0) {
			kos_bda_implementation.temp_value = get_device_keyboard_key;
			get_device_keyboard_key = 0;
			
		} else if (strcmp(data, "press key") == 0) {
			kos_bda_implementation.temp_value = get_device_keyboard_keycode;
			get_device_keyboard_keycode = 0;
			
		} else {
			KOS_DEVICE_COMMAND_WARNING("keyboard")
			
		}
		
		*result = (unsigned long long*) &kos_bda_implementation.temp_value;
		
	}
	
#endif
