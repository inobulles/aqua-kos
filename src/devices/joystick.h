
#ifndef __AQUA__KOS_DEVICES_JOYSTICK_H
	#define __AQUA__KOS_DEVICES_JOYSTICK_H
	
	#define MAX_JOYSTICK_COUNT 64
	
	static unsigned long long kos_joystick_count = 0;
	
	#if KOS_USES_SDL2
		static SDL_GameController* kos_sdl2_joysticks[MAX_JOYSTICK_COUNT];
	#endif
	
	void kos_open_joysticks(void) {
		#if KOS_USES_SDL2
			kos_joystick_count = SDL_NumJoysticks();
			
			for (unsigned long long i = 0; i < kos_joystick_count; i++) {
				if (SDL_IsGameController(i)) kos_sdl2_joysticks[i] = SDL_GameControllerOpen(i);
				else                         kos_sdl2_joysticks[i] = (SDL_GameController*) 0;
				
			}
		#endif
		
	}
	
	void kos_close_joysticks(void) {
		#if KOS_USES_SDL2
			for (unsigned long long i = 0; i < kos_joystick_count; i++) {
				if (kos_sdl2_joysticks[i]) {
					SDL_GameControllerClose(kos_sdl2_joysticks[i]);
					
				}
				
			}
		#endif
		
	}
	
	const char* kos_get_joystick_name(unsigned long long __this) {
		#if KOS_USES_SDL2
			return SDL_JoystickName(SDL_GameControllerGetJoystick(kos_sdl2_joysticks[__this]));
		#else
			return (char*) 0;
		#endif
		
	}
	
	unsigned long long kos_get_joystick_button(unsigned long long __this, unsigned long long button) {
		#if KOS_USES_SDL2
			return SDL_GameControllerGetButton(kos_sdl2_joysticks[__this], button);
		#else
			return 0;
		#endif
		
	} float kos_get_joystick_axis(unsigned long long __this, unsigned long long axis) {
		#if KOS_USES_SDL2
			return (float) SDL_GameControllerGetAxis(kos_sdl2_joysticks[__this], axis) / 32768.0f;
		#else
			return 0.0f;
		#endif
		
	}
	
	static void joystick_device_handle(unsigned long long** result, const char* data) {
		unsigned long long* command = (unsigned long long*) data;
		unsigned char result_string = 0;
		
		if (command[0] == 'n') {
			result_string = 1;
			strncpy(kos_bda_implementation.temp_string, kos_get_joystick_name(command[1]), sizeof(kos_bda_implementation.temp_string));
			
		}
		
		else if (command[0] == 'c') kos_bda_implementation.temp_value = kos_joystick_count;
		else if (command[0] == 'b') kos_bda_implementation.temp_value = kos_get_joystick_button(command[1], command[2]);
		else if (command[0] == 'a') kos_bda_implementation.temp_value = (unsigned long long) (kos_get_joystick_axis(command[1], command[2]) * FLOAT_ONE);
		
		else if (command[0] == 'h') { /// TODO clean up
			SDL_Haptic* haptic = SDL_HapticOpenFromJoystick(SDL_GameControllerGetJoystick(kos_sdl2_joysticks[command[1]]));
			SDL_HapticRumblePlay(haptic, 0.5f, 2000);
			SDL_HapticClose(haptic);
			
		}
		
		else KOS_DEVICE_COMMAND_WARNING("joystick")
		*result =  result_string ? (unsigned long long*) kos_bda_implementation.temp_string : &kos_bda_implementation.temp_value;
		
	}
	
#endif
