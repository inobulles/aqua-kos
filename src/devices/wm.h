
#ifndef __AQUA__KOS_DEVICES_WM_H
	#define __AQUA__KOS_DEVICES_WM_H
	
	static void wm_device_handle(unsigned long long** result, const char* data) {
		if (strncmp(data, "visible ", 8) == 0) {
			if (!atoi(data + 8)) {
				#if KOS_USES_SDL2
					SDL_MinimizeWindow(current_kos->window);
				#endif
				
			}
			
		} else {
			KOS_DEVICE_COMMAND_WARNING("wm")
			
		}
		
	}
	
#endif
