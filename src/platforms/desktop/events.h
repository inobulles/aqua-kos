
void get_events(events_t* events) {
	mouse_scroll = 0;
	
	SDL_Event event;
	SDL_PumpEvents();
	
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			events->quit = 1;
			
		} else if (event.type == SDL_WINDOWEVENT) {
			if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
				video_width  = event.window.data1;
				video_height = event.window.data2;
				
				events->resize = 1;
				glViewport(0, 0, video_width, video_height);
			}
			
		} else if (event.type == SDL_MOUSEWHEEL) {
			mouse_scroll = event.wheel.y * PRECISION;
		}
	}
	
	mouse_button = SDL_GetMouseState((int*) &mouse_x, (int*) &mouse_y);
	
	mouse_x = (mouse_x & 0xFFFFFFFF) * PRECISION / video_width  * 2 - PRECISION;
	mouse_y = (mouse_y & 0xFFFFFFFF) * PRECISION / video_height * 2 - PRECISION;
}
