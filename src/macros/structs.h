
#ifndef __AQUA__SDL2_SRC_MACROS_AND_INCLUSIONS_STRUCTS_H
	#define __AQUA__SDL2_SRC_MACROS_AND_INCLUSIONS_STRUCTS_H

	#if KOS_USES_OPENGLES
		#include <EGL/egl.h>
		#include <GLES2/gl2.h>
	#endif

	typedef struct {
		int warning_count;
		int text_only;
		
		int width;
		int height;
		
		int bpp;
		
		#if KOS_USES_SDL2
			SDL_Window*   window;
			SDL_GLContext context;
		#endif
		
		#if KOS_USES_OPENGL
			GLuint shader_program;
		#endif
		
		#if KOS_USES_OPENGLES
			EGLContext context;
			
			#if KOS_USES_BCM
				EGLDisplay display;
				EGLSurface surface;
			#endif
		
			#if KOS_USES_JNI
				char*   vertex_shader;
				char* fragment_shader;
			#endif
		#endif
		
		#ifdef __HAS_AUDIO
			struct mad_stream stream;
			struct mad_synth  synth;
			struct mad_frame  frame;
		#endif
		
	} kos_t;
	
#endif
