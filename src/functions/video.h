
#ifndef __AQUA__SDL2_SRC_FUNCTIONS_VIDEO_H
	#define __AQUA__SDL2_SRC_FUNCTIONS_VIDEO_H
	
	#include "../macros_and_inclusions.h"
	#include "../lib/structs.h"
	
	#if KOS_USES_ANDROID
		#include "../android/root.h"
	#endif

	void video_clear(void) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
	} void video_clear_colour(unsigned long long r, unsigned long long g, unsigned long long b, unsigned long long a) {
		glClearColor((float) r / _UI64_MAX, (float) g / _UI64_MAX, (float) b / _UI64_MAX, (float) a / _UI64_MAX);
		
	}
	
	#if KOS_USES_JNI
		unsigned char waiting_video_flip = 0;
	#endif
	
	void __update_predefined_texture(unsigned long long name);
	static unsigned char predefined_textures_live = 0;
	
	static unsigned char video_flip_called = 0;
	
	#include <time.h>
	
	static unsigned long long last_ns = 0;
	static unsigned long long fps = 0;
	
	void video_flip(void) {
		// get current fps
		
		struct timespec now;
		clock_gettime(CLOCK_MONOTONIC, &now);
		unsigned long long ns = now.tv_sec * 1000000000ull + now.tv_nsec;
		
		if (last_ns > 0) {
			float delta = (float) (ns - last_ns) * 0.000000001f;
			fps = (unsigned long long) (1.0f / delta);
			
		} last_ns = ns;
		
		// flip
		
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
		
		if (predefined_textures_live) { // update predefined texture if live
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
		return fps;
		
	}
	
	// include some stuff to make life easier
	
	#include "events.h"
	#include "predefined.h"
	#include "screenshot.h"
	
#endif
