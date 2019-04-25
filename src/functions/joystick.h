
#ifndef __AQUA__KOS__SRC_FUNCTIONS_JOYSTICK_H
	#define __AQUA__KOS__SRC_FUNCTIONS_JOYSTICK_H
	
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
		
	}
	
#endif
