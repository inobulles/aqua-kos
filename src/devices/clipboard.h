
#ifndef __AQUA__KOS_DEVICES_CLIPBOARD_H
	#define __AQUA__KOS_DEVICES_CLIPBOARD_H
	
	static inline unsigned long long get_clipboard_count(void) {
		#if KOS_USES_SDL2
			return 1;
		#else
			return 0;
		#endif
		
	}
	
	void* heap_malloc(unsigned long long bytes);
	
	static void clipboard_device_handle(unsigned long long** result, const char* data) {
		unsigned long long* command = (unsigned long long*) data;
		kos_bda_implementation.temp_value = 0;
		*result = (unsigned long long*) &kos_bda_implementation.temp_value;
		
		if (command[0] == 'c') {
			kos_bda_implementation.temp_value = get_clipboard_count();
			
		} else if (command[0] == 'g') {
			char* buffer = (char*) 0;
			
			#if KOS_USES_SDL2
				buffer = SDL_GetClipboardText();
			#endif
			
			unsigned long long bytes = buffer ? strlen(buffer) + 1 : 1;
			kos_bda_implementation.temp_value = (unsigned long long) heap_malloc(bytes);
			memcpy((void*) kos_bda_implementation.temp_value, bytes == 1 ? "" : buffer, bytes);
			
		} else if (command[0] == 's') {
			kos_bda_implementation.temp_value = 1;
			
			#if KOS_USES_SDL2
				kos_bda_implementation.temp_value = SDL_SetClipboardText((const char*) command[1]) != 0;
			#endif
			
		} else {
			KOS_DEVICE_COMMAND_WARNING("keyboard");
			
		}
		
	}
	
#endif
