
#ifndef __AQUA__SDL2_SRC_MACROS_AND_INCLUSIONS_WARNINGS_H
	#define __AQUA__SDL2_SRC_MACROS_AND_INCLUSIONS_WARNINGS_H
	
	#ifndef KOS_WARN_NO_GL_VERSION
		#define KOS_WARN_NO_GL_VERSION printf("WARNING You seem to have an inexistant OpenGL version (%d.%d)\n", kos_best_gl_version_major, kos_best_gl_version_minor);
	#endif
	
#endif
