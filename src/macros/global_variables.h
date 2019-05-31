
#ifndef __AQUA__SDL2_SRC_MACROS_AND_INCLUSIONS_GLOBAL_VARIABLES_H
	#define __AQUA__SDL2_SRC_MACROS_AND_INCLUSIONS_GLOBAL_VARIABLES_H
	
	static kos_t* current_kos;
	
	static char kos_best_gl_version_major;
	static char kos_best_gl_version_minor;
	
	static const float vertex_matrix[] = {
		0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
	};
	
	static const float texture_coords[] = {
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
	};
	
#endif
