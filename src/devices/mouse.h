
#ifndef __AQUA__KOS_DEVICES_MOUSE_H
	#define __AQUA__KOS_DEVICES_MOUSE_H

	static unsigned long long kos_get_mouse_count(void) {
		#if KOS_USES_SDL2
			return 1;
		#elif KOS_USES_JNI
			return (unsigned long long) CALLBACK_INT(java_mouse_count, 0);
		#else
			return 0;
		#endif
		
	} static unsigned long long kos_get_mouse_button(unsigned long long self, unsigned long long button) {
		#if KOS_USES_SDL2
			SDL_PumpEvents();
			return SDL_GetMouseState((int*) 0, (int*) 0) & SDL_BUTTON(button);
		#elif KOS_USES_JNI
			return (unsigned long long) CALLBACK(java_mouse_button, callback_env->CallStaticIntMethod, (jint) self, (jint) button);
		#else
			return 0;
		#endif
		
	}
	
	static unsigned long long kos_get_mouse_x(unsigned long long self) {
		#if KOS_USES_SDL2
			SDL_PumpEvents();
			int x;
			SDL_GetMouseState(&x, (int*) 0);
			return (x * FLOAT_ONE) / video_width() * 2 - FLOAT_ONE;
		#elif KOS_USES_JNI
			return (unsigned long long) (signed long long) (((float) CALLBACK_INT(java_mouse_x, (jint) self) / video_width() * FLOAT_ONE) * 2 - FLOAT_ONE);
		#else
			return 0;
		#endif
		
	} static unsigned long long kos_get_mouse_y(unsigned long long self) {
		#if KOS_USES_SDL2
			SDL_PumpEvents();
			int y;
			SDL_GetMouseState((int*) 0, &y);
			return -((y * FLOAT_ONE) / video_height() * 2 - FLOAT_ONE);
		#elif KOS_USES_JNI
			return (unsigned long long) (signed long long) -(((float) CALLBACK_INT(java_mouse_y, (jint) self) / video_height() * FLOAT_ONE) * 2 - FLOAT_ONE);
		#else
			return 0;
		#endif
		
	}
	
	static void mouse_device_handle(unsigned long long** result, const char* data) {
		unsigned long long* command = (unsigned long long*) data;
		
		if      (command[0] == 'c') kos_bda_implementation.temp_value = kos_get_mouse_count();
		else if (command[0] == 'b') kos_bda_implementation.temp_value = kos_get_mouse_button(command[1], command[2]);
		
		else if (command[0] == 'x') kos_bda_implementation.temp_value = kos_get_mouse_x(command[1]);
		else if (command[0] == 'y') kos_bda_implementation.temp_value = kos_get_mouse_y(command[1]);
		
		else KOS_DEVICE_COMMAND_WARNING("mouse")
		
		*result = &kos_bda_implementation.temp_value;
		
	}
	
#endif
