
#ifndef __AQUA__KOS_DEVICES_JOYSTICK_H
	#define __AQUA__KOS_DEVICES_JOYSTICK_H
	
	#define MAX_JOYSTICK_COUNT 64
	
	static unsigned long long kos_joystick_count   = 0;
	static unsigned long long kos_current_joystick = 0;
	
	#if KOS_USES_SDL2
		static SDL_Joystick* kos_sdl2_joysticks[MAX_JOYSTICK_COUNT];
	#endif
	
	void kos_open_joysticks(void) {
		#if KOS_USES_SDL2
			kos_joystick_count = SDL_NumJoysticks();
			
			for (unsigned long long i = 0; i < kos_joystick_count; i++) {
				kos_sdl2_joysticks[i] = SDL_JoystickOpen(i);
				
			}
		#endif
		
	}
	
	void kos_close_joysticks(void) {
		#if KOS_USES_SDL2
			for (unsigned long long i = 0; i < kos_joystick_count; i++) {
				SDL_JoystickClose(kos_sdl2_joysticks[i]);
				
			}
		#endif
		
	}
	
	void kos_select_joystick(unsigned long long __this) {
		if (__this < kos_joystick_count) {
			kos_current_joystick = __this;
			
		}
		
	}
	
	const char* kos_get_joystick_name(unsigned long long __this) {
		#if KOS_USES_SDL2
			return SDL_JoystickName(kos_sdl2_joysticks[__this]);
		#else
			return (char*) 0;
		#endif
		
	}
	
	unsigned long long kos_joystick_button_count(unsigned long long __this) {
		#if KOS_USES_SDL2
			return SDL_JoystickNumButtons(kos_sdl2_joysticks[__this]);
		#endif
		
	} unsigned long long kos_joystick_axis_count(unsigned long long __this) {
		#if KOS_USES_SDL2
			return SDL_JoystickNumAxes(kos_sdl2_joysticks[__this]);
		#endif
		
	} unsigned long long kos_joystick_hat_count(unsigned long long __this) {
		#if KOS_USES_SDL2
			return SDL_JoystickNumHats(kos_sdl2_joysticks[__this]);
		#endif
		
	}
	
	unsigned long long kos_get_joystick_button(unsigned long long __this, unsigned long long button) {
		#if KOS_USES_SDL2
			return SDL_JoystickGetButton(kos_sdl2_joysticks[__this], button);
		#else
			return 0;
		#endif
		
	} unsigned long long kos_get_joystick_hat(unsigned long long __this, unsigned long long hat) {
		#if KOS_USES_SDL2
			return SDL_JoystickGetHat(kos_sdl2_joysticks[__this], hat);
		#else
			return 0;
		#endif
		
	} float kos_get_joystick_axis(unsigned long long __this, unsigned long long axis) {
		#if KOS_USES_SDL2
			return (float) SDL_JoystickGetAxis(kos_sdl2_joysticks[__this], axis) / 32768.0f;
		#else
			return 0.0f;
		#endif
		
	}
	
	static void joystick_device_handle(unsigned long long** result, const char* data) {
		unsigned char result_string = 0;
		
		if (strcmp(data, "name") == 0) {
			strncpy(kos_bda_implementation.temp_string, kos_get_joystick_name(kos_current_joystick), sizeof(kos_bda_implementation.temp_string));
			result_string = 1;
			
		} else if (strncmp(data, "select ", 7) == 0) {
			kos_select_joystick(atoi(data + 7));
			
		}
		
		else if (strcmp (data, "count"       ) == 0) kos_bda_implementation.temp_value = kos_joystick_count;
		else if (strcmp (data, "button count") == 0) kos_bda_implementation.temp_value = kos_joystick_button_count(kos_current_joystick);
		else if (strcmp (data, "axis count"  ) == 0) kos_bda_implementation.temp_value = kos_joystick_axis_count  (kos_current_joystick);
		else if (strcmp (data, "hat count"   ) == 0) kos_bda_implementation.temp_value = kos_joystick_hat_count   (kos_current_joystick);
		
		else if (strncmp(data, "button ",   7) == 0) kos_bda_implementation.temp_value =                       kos_get_joystick_button(kos_current_joystick, atoi(data + 7));
		else if (strncmp(data, "hat ",      4) == 0) kos_bda_implementation.temp_value =                       kos_get_joystick_hat   (kos_current_joystick, atoi(data + 4));
		else if (strncmp(data, "axis ",     5) == 0) kos_bda_implementation.temp_value = (unsigned long long) (kos_get_joystick_axis  (kos_current_joystick, atoi(data + 5)) * FLOAT_ONE);
		
		else KOS_DEVICE_COMMAND_WARNING("joystick")
		*result =  result_string ? (unsigned long long*) kos_bda_implementation.temp_string : &kos_bda_implementation.temp_value;
		
	}
	
#endif
