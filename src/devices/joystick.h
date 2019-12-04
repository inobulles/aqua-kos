
#ifndef __AQUA__KOS_DEVICES_JOYSTICK_H
	#define __AQUA__KOS_DEVICES_JOYSTICK_H
	
	#define MAX_JOYSTICK_COUNT 64
	
	static unsigned long long kos_joystick_count = 0;
	
	#if KOS_USES_SDL2
		static struct {
			SDL_GameController* game_controller;
			SDL_Joystick* joystick;
			SDL_Haptic* haptic;
		} kos_sdl2_joysticks[MAX_JOYSTICK_COUNT];
	#endif
	
	static inline unsigned long long kos_get_joystick_count(void) {
		#if KOS_USES_JNI
			return (unsigned long long) CALLBACK(java_joystick_count, callback_env->CallStaticIntMethod, 0);
		#else
			return kos_joystick_count;
		#endif
		
	}

	void kos_open_joysticks(void) {
		#if KOS_USES_SDL2
			kos_joystick_count = SDL_NumJoysticks();
			
			for (unsigned long long i = 0; i < kos_joystick_count; i++) {
				if (SDL_IsGameController(i)) {
					kos_sdl2_joysticks[i].game_controller = SDL_GameControllerOpen(i);
					kos_sdl2_joysticks[i].joystick = SDL_GameControllerGetJoystick(kos_sdl2_joysticks[i].game_controller);
				} else {
					kos_sdl2_joysticks[i].game_controller = (SDL_GameController*) 0;
					kos_sdl2_joysticks[i].joystick = SDL_JoystickOpen(i);
				}
				
				if ((kos_sdl2_joysticks[i].haptic = SDL_HapticOpenFromJoystick(kos_sdl2_joysticks[i].joystick)) != (SDL_Haptic*) 0) {
					SDL_HapticRumbleInit(kos_sdl2_joysticks[i].haptic);
				} else {
					kos_sdl2_joysticks[i].haptic = 0;
				}
			}
		#endif
		
	}
	
	void kos_close_joysticks(void) {
		#if KOS_USES_SDL2
			for (unsigned long long i = 0; i < kos_joystick_count; i++) {
				if (kos_sdl2_joysticks[i].game_controller) SDL_GameControllerClose(kos_sdl2_joysticks[i].game_controller);
				else if (kos_sdl2_joysticks[i].joystick) SDL_JoystickClose(kos_sdl2_joysticks[i].joystick);
				if (kos_sdl2_joysticks[i].haptic) SDL_HapticClose(kos_sdl2_joysticks[i].haptic);
			}
		#endif
		
	}
	
	static inline const char* kos_get_joystick_name(unsigned long long __this) {
		#if KOS_USES_JNI
			jboolean is_copy = 0;
			return callback_env->GetStringUTFChars((jstring) CALLBACK(java_joystick_name, callback_env->CallStaticObjectMethod, __this), &is_copy);
		#elif KOS_USES_SDL2
			return SDL_JoystickName(kos_sdl2_joysticks[__this].joystick);
		#else
			return (char*) 0;
		#endif
		
	}
	
	static inline unsigned long long kos_get_joystick_button(unsigned long long __this, unsigned long long button) {
		#if KOS_USES_JNI
			return (unsigned long long) CALLBACK(java_joystick_button, callback_env->CallStaticBooleanMethod, __this);
		#elif KOS_USES_SDL2
			if (kos_sdl2_joysticks[__this].game_controller) return SDL_GameControllerGetButton(kos_sdl2_joysticks[__this].game_controller, button);
			else return SDL_JoystickGetButton(kos_sdl2_joysticks[__this].joystick, button);
		#else
			return 0;
		#endif
		
	} static inline float kos_get_joystick_axis(unsigned long long __this, unsigned long long axis) {
		#if KOS_USES_JNI
			return (float) CALLBACK(java_joystick_axis, callback_env->CallStaticFloatMethod, __this);
		#elif KOS_USES_SDL2
			if (kos_sdl2_joysticks[__this].game_controller) return (float) SDL_GameControllerGetAxis(kos_sdl2_joysticks[__this].game_controller, axis) / 32768.0f;
			else return (float) SDL_JoystickGetAxis(kos_sdl2_joysticks[__this].joystick, axis) / 32768.0f;
		#else
			return 0.0f;
		#endif
		
	}
	
	static inline unsigned long long kos_joystick_rumble(unsigned long long __this, unsigned long long strength, unsigned long long seconds) {
		#if KOS_USES_JNI
			printf("TODO Joystick rumble on JNI\n");
		#elif KOS_USES_SDL2
			if (kos_sdl2_joysticks[__this].haptic) return SDL_HapticRumblePlay(kos_sdl2_joysticks[__this].haptic, (double) strength / FLOAT_ONE, seconds / 1000);
			else return 1;
		#endif
	}
	
	static void joystick_device_handle(unsigned long long** result, const char* data) {
		unsigned long long* command = (unsigned long long*) data;
		unsigned char result_string = 0;
		
		if (command[0] == 'n') {
			result_string = 1;
			strncpy(kos_bda_implementation.temp_string, kos_get_joystick_name(command[1]), sizeof(kos_bda_implementation.temp_string));
			
		}
		
		else if (command[0] == 'c') kos_bda_implementation.temp_value = kos_get_joystick_count();
		else if (command[0] == 'b') kos_bda_implementation.temp_value = kos_get_joystick_button(command[1], command[2]);
		else if (command[0] == 'a') kos_bda_implementation.temp_value = (unsigned long long) (kos_get_joystick_axis(command[1], command[2]) * FLOAT_ONE);
		else if (command[0] == 'h') kos_bda_implementation.temp_value = kos_joystick_rumble(command[1], command[2], command[3]);
		
		else KOS_DEVICE_COMMAND_WARNING("joystick")
		*result = result_string ? (unsigned long long*) kos_bda_implementation.temp_string : &kos_bda_implementation.temp_value;
		
	}
	
#endif
