
#ifndef __AQUA__SDL2_SRC_GL_COMMON_SHADERS_H
	#define __AQUA__SDL2_SRC_GL_COMMON_SHADERS_H
	
	static unsigned char shader_has_set_locations;
	
	static GLint shader_mvp_matrix_location;
	static GLint shader_time_location;
	
	static GLint shader_sampler_location;
	static GLint shader_has_texture_location;
	
	static GLint shader_width_location;
	static GLint shader_height_location;
	
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
	
	unsigned long long gl_shader_get_uniform_location(GLuint* program, const char* string) {
		switch (kos_best_gl_version_major) {
			case 2: return gl2_shader_get_uniform_location(program, string); break;
			default: return 0;
		}
	}
	
	void gl_shader_set_uniform_location(GLint uniform_location, unsigned long long data, unsigned long long type) {
		switch (kos_best_gl_version_major) {
			case 2: gl2_shader_set_uniform_location(uniform_location, data, type); break;
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
