
#ifndef __AQUA__KOS_FUNCTIONS_EVENTS_H
	#define __AQUA__KOS_FUNCTIONS_EVENTS_H
	
	static unsigned char kos_is_mouse_pressed = 0;
	static unsigned char kos_has_clicked = 0;
	
	extern unsigned long long get_device_keyboard_key;
	extern unsigned long long get_device_keyboard_keycode;
	
	static unsigned long long resize_count;
	static unsigned char      first_event_flush = 1;

	#if KOS_USES_JNI
		static int event_pointer_x = 0;
		static int event_pointer_y = 0;

		static int event_pointer_click_type = 0;
		static int event_quit               = 0;

		static unsigned char has_the_event_been_updated_in_the_previous_call_to_Java_com_inobulles_obiwac_aqua_Lib_event_question_mark = 0;
		static int event_last_release = 1;
	#endif

	int gl_resize = 0;

	void get_events(unsigned long long ____this) { // I guess this shouldn't be here but idc tbh
		event_list_t* __this = (event_list_t*) ____this;
		
		unsigned long long half_width  = (unsigned long long) (current_kos->width  >> 1);
		unsigned long long half_height = (unsigned long long) (current_kos->height >> 1);
		
		__this->pointer_click_type = 0;
		
		__this->quit   = 0;
		__this->resize = 0;
		
		if (first_event_flush) {
			__this->pointer_x = half_width;
			__this->pointer_y = half_height;
			
			first_event_flush = 0;
			memset(__this, 0, sizeof(event_list_t));
			
		}
		
		// RETURN HERE IF EVENTS SHOULD BE IGNORED
		
		__this->pointer_click_type = kos_is_mouse_pressed;
		
		#if KOS_USES_SDL2
			SDL_Event event;
			SDL_PumpEvents();
			
			while (SDL_PollEvent(&event)) {
				if (event.type == SDL_QUIT) {
					__this->quit = 1;
					break;
					
				} else if (event.type == SDL_WINDOWEVENT) {
					switch (event.window.event) {
						case SDL_WINDOWEVENT_SIZE_CHANGED: {
							resize_count++;
							__this->resize = 1;
							
							current_kos->width  = event.window.data1;
							current_kos->height = event.window.data2;
							
							glViewport(0, 0, current_kos->width, current_kos->height);
							update_all_font_sizes();
							
							break;
							
						} default: {
							break;
							
						}
						
					}
					
					break;
					
				} else if (event.type == SDL_MOUSEMOTION) {
					__this->pointer_x = event.motion.x;
					__this->pointer_y = event.motion.y;
					
				} else if (event.type == SDL_MOUSEBUTTONDOWN) {
					__this->pointer_x = event.button.x;
					__this->pointer_y = event.button.y;
					
					kos_has_clicked = 1;
					kos_is_mouse_pressed = 1;
					
					break;
					
				} else if (event.type == SDL_MOUSEBUTTONUP) {
					__this->pointer_x = event.button.x;
					__this->pointer_y = event.button.y;
					
					kos_is_mouse_pressed = 0;
					break;
					
				} else if (event.type == SDL_KEYDOWN) {
					get_device_keyboard_key = event.key.keysym.scancode;
					break;
					
				} else if (event.type == SDL_TEXTINPUT) {
					get_device_keyboard_keycode = *event.text.text;
					break;
					
				}
				
			}
		#elif KOS_USES_JNI
			__this->quit   = (unsigned long long) event_quit;
			__this->resize = (unsigned long long) gl_resize;

			__this->pointer_click_type = (unsigned long long) !event_last_release;
			has_the_event_been_updated_in_the_previous_call_to_Java_com_inobulles_obiwac_aqua_Lib_event_question_mark = 0;
		
			__this->pointer_x = (unsigned long long) event_pointer_x;
			__this->pointer_y = (unsigned long long) event_pointer_y;
			
			gl_resize = 0;
		#endif

		__this->pointer_x = __this->pointer_x >= video_width()  ? half_width  : __this->pointer_x;
		__this->pointer_y = __this->pointer_y >= video_height() ? half_height : __this->pointer_y;
		
	}
	
#endif