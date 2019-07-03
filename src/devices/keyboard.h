
#ifndef __AQUA__KOS_DEVICES_KEYBOARD_H
	#define __AQUA__KOS_DEVICES_KEYBOARD_H
	
	static inline unsigned long long get_keyboard_count(void) {
		#if KOS_USES_SDL2
			return 1;
		#else
			return 0;
		#endif
		
	} static inline unsigned long long get_keyboard_scancode(unsigned long long self, unsigned long long key) {
		#if KOS_USES_SDL2
			SDL_PumpEvents();
			const uint8_t* scancodes = SDL_GetKeyboardState(NULL);
			return scancodes[key];
		#endif
		
		return 0;
		
	}
	
	unsigned long long get_device_keyboard_key     = 0;
	unsigned long long get_device_keyboard_keycode = 0;
	
	static void keyboard_device_handle(unsigned long long** result, const char* data) {
		unsigned long long* command = (unsigned long long*) data;
		kos_bda_implementation.temp_value = 0;
		*result = (unsigned long long*) &kos_bda_implementation.temp_value;
		
		if      (command[0] == 'c') kos_bda_implementation.temp_value = get_keyboard_count();
		else if (command[0] == 's') kos_bda_implementation.temp_value = get_keyboard_scancode(command[1], command[2]);
		else KOS_DEVICE_COMMAND_WARNING("keyboard");
		
	}
	
#endif
