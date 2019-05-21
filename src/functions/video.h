
#ifndef __AQUA__SDL2_SRC_FUNCTIONS_VIDEO_H
	#define __AQUA__SDL2_SRC_FUNCTIONS_VIDEO_H
	
	#include "../macros_and_inclusions.h"
	#include "../lib/structs.h"
	
	#if KOS_USES_ANDROID
		#include "../android/root.h"
	#endif

	void video_clear(void) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
	}
	
	void video_clear_colour(unsigned long long r, unsigned long long g, unsigned long long b, unsigned long long a) {
		glClearColor((float) r / _UI64_MAX, (float) g / _UI64_MAX, (float) b / _UI64_MAX, (float) a / _UI64_MAX);
		
	}
	
	void video_draw(void) {
		printf("WARNING __this function (`video_draw`) is deprecated\n");
		
	}

	#if KOS_USES_JNI
		unsigned char waiting_video_flip = 0;
	#endif
	
	void __update_predefined_texture(unsigned long long name);
	static unsigned char predefined_textures_live = 0;
	
	static surface_t predefined_texture_surface_dummy;
	static unsigned char video_flip_called = 0;
	
	static unsigned long long kos_last_time;
	static unsigned long long kos_last_fps;
	
	void video_flip(void) {
		video_flip_called = 1;
		
		#if KOS_USES_SDL2 && KOS_USES_OPENGL
			SDL_GL_SwapWindow(current_kos->window);
		#endif
		
		#if KOS_USES_BCM && KOS_USES_OPENGLES
			eglSwapBuffers(current_kos->display, current_kos->surface);
		#endif
		
		#if KOS_USES_JNI
			waiting_video_flip = 1;
		#endif
		
		surface_layer_offset = 0.0f;
		
		#if KOS_3D_VISUALIZATION
			glRotatef(1.0f, 0.0f, 1.0f, 0.0f);
		#endif
		
		if (predefined_textures_live) {
			__update_predefined_texture(TEXTURE_BACKGROUND);
			__update_predefined_texture(TEXTURE_FROSTED_BACKGROUND);
			
		}
		
	}
	
	unsigned long long video_width (void) { return (unsigned long long) current_kos->width;  }
	unsigned long long video_height(void) { return (unsigned long long) current_kos->height; }
	
	unsigned long long video_bpp(void) {
		return (unsigned long long) current_kos->bpp;
		
	}
	
	unsigned long long video_fps(void) {
		#if KOS_USES_JNI
			extern unsigned long long gl_fps;
			kos_last_fps = gl_fps;
		#elif KOS_USES_SDL2
			unsigned long long tick_time = SDL_GetTicks();
			
			kos_last_fps  = (unsigned long long) (1000.0f / (float) (tick_time - kos_last_time));
			kos_last_time = tick_time;
		#endif
		
		if (kos_last_fps <= 1) {
			kos_last_fps  = 60; /// TODO find the most appropriate framerate
			
		}
		
		return kos_last_fps;
		
	}
	
	void set_video_visibility(unsigned long long state) {
		KOS_DEPRECATED
		
		if (state == HIDDEN) {
			#if KOS_USES_SDL2
				SDL_MinimizeWindow(current_kos->window);
			#endif
			
		}
		
	}
	
	// include some stuff to make life easier
	
	#include "events.h"
	#include "predefined.h"
	#include "screenshot.h"
	
#endif
