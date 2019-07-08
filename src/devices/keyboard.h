
#ifndef __AQUA__KOS_DEVICES_KEYBOARD_H
	#define __AQUA__KOS_DEVICES_KEYBOARD_H
	
	static unsigned long long text_input_buffer_bytes = 0;
	static char* text_input_buffer = (char*) 0;
	
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
	
	void* heap_malloc(unsigned long long bytes);
	
	static void keyboard_device_handle(unsigned long long** result, const char* data) {
		unsigned long long* command = (unsigned long long*) data;
		kos_bda_implementation.temp_value = 0;
		*result = (unsigned long long*) &kos_bda_implementation.temp_value;
		
		if      (command[0] == 'c') kos_bda_implementation.temp_value = get_keyboard_count();
		else if (command[0] == 's') kos_bda_implementation.temp_value = get_keyboard_scancode(command[1], command[2]);
		
		else if (command[0] == 'i') {
			unsigned long long bytes = text_input_buffer_bytes ? text_input_buffer_bytes : 1;
			kos_bda_implementation.temp_value = (unsigned long long) heap_malloc(bytes);
			memcpy((void*) kos_bda_implementation.temp_value, bytes == 1 ? "" : text_input_buffer, bytes);
			
			mfree(text_input_buffer, text_input_buffer_bytes);
			text_input_buffer_bytes = 0;
			text_input_buffer = (char*) 0;
			
		} else {
			KOS_DEVICE_COMMAND_WARNING("keyboard");
			
		}
		
	}
	
#endif
