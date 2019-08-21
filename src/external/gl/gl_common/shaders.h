
#ifndef __AQUA__SDL2_SRC_GL_COMMON_SHADERS_H
	#define __AQUA__SDL2_SRC_GL_COMMON_SHADERS_H
	
	static unsigned char shader_has_set_locations;
	
	static GLint shader_mvp_matrix_location;
	static GLint shader_time_location;
	
	static GLint shader_sampler_location;
	static GLint shader_aux_location;
	static GLint shader_has_texture_location;
	
	static GLint shader_width_location;
	static GLint shader_height_location;
	
	static GLint shader_threshold_location;
	static GLint shader_radius_location;
	
	#include "../gl_versions/shaders/gl_1.h"
	#include "../gl_versions/shaders/gl_2.h"
	#include "../gl_versions/shaders/gl_3.h"
	#include "../gl_versions/shaders/gl_4.h"
	
	int gl_load_shaders(GLuint* program, char* vertex_code, char* fragment_code) {
		switch (kos_best_gl_version_major) {
			case 1: return 1;
			case 2: return gl2_load_shaders(program, vertex_code, fragment_code);
			case 3: return 1;
			case 4: return 1;
			case 5: return 1;
			
			default: {
				KOS_WARN_NO_GL_VERSION
				return 1;
				
			}
			
		}
		
	}
	
	int glGetUniformLocation();
	
	void gl_use_shader_program(GLuint* program) {
		switch (kos_best_gl_version_major) {
			case 1: break;
			case 2: gl2_use_shader_program(program); break;
			case 3: break;
			case 4: break;
			case 5: break;
			
			default: {
				KOS_WARN_NO_GL_VERSION
				break;
				
			}
			
		}
		
	}
	
	void gl_shader_locations(GLuint aux, GLfloat radius, GLfloat threshold) {
		switch (kos_best_gl_version_major) {
			case 1: break;
			case 2: gl2_shader_locations(aux, radius, threshold); break;
			case 3: break;
			case 4: break;
			case 5: break;
			
			default: {
				KOS_WARN_NO_GL_VERSION
				break;
				
			}
			
		}
		
	}
	
	void gl_delete_shader_program(GLuint* program) {
		switch (kos_best_gl_version_major) {
			case 1: break;
			case 2: gl2_delete_shader_program(program); break;
			case 3: break;
			case 4: break;
			case 5: break;
			
			default: {
				KOS_WARN_NO_GL_VERSION
				break;
				
			}
			
		}
		
	}
	
#endif
