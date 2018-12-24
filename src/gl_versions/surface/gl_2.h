
#ifndef __AQUA__SDL2_SRC_KOS_GL_VERSIONS_SURFACE_GL_2_H
	#define __AQUA__SDL2_SRC_KOS_GL_VERSIONS_SURFACE_GL_2_H
	
	#include "../../macros_and_inclusions.h"
#include "../../gl_common/shaders.h"
#include "../../lib/structs.h"
#include <time.h>
	
	int glUniform1i();
	int glUniform1f();

	#if KOS_USES_JNI
		extern GLfloat projection_matrix[16];

		static inline void vertex_attribute_pointer(GLuint index, int vector_size, void* pointer) {
			glVertexAttribPointer(index, vector_size, GL_FLOAT, GL_FALSE, vector_size * sizeof(GLfloat), pointer);
			
		}
	#endif

	static inline void gl2_surface_draw_shader_sampler_location(surface_t* __this) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture  (GL_TEXTURE_2D, (GLuint) __this->texture);
		
		if (shader_has_set_locations) {
			glUniform1i(shader_sampler_location, 0);
			
		}
		
	}

	int gl2_surface_draw(surface_t* __this) {
		if (shader_has_set_locations) {
			glUniform1i(shader_time_location, (GLint) clock());
			glUniform1i(shader_has_texture_location, (GLint) __this->has_texture);
			
		}
		
		#if KOS_USES_OPENGL_DESKTOP
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_COLOR_ARRAY);

			if (__this->has_texture) {
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);

			}

			glVertexPointer(3, GL_FLOAT, 0, __this->vertices);
			glColorPointer (4, GL_FLOAT, 0, __this->colours);

			if (__this->has_texture) {
				glTexCoordPointer(2, GL_FLOAT, 0, __this->texture_coords);
				gl2_surface_draw_textures(__this);

			}

			glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_BYTE, __this->faces);

			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_COLOR_ARRAY);

			if (__this->has_texture) {
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);

			}
		#elif KOS_USES_JNI
			if (!shader_has_set_locations) {
				printf("WARNING Shader has not yet set locations\n");
				
			}
		
			vertex_attribute_pointer(0, 3, __this->vertices);
			vertex_attribute_pointer(1, 2, __this->texture_coords);
			vertex_attribute_pointer(2, 4, __this->colours);
		
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
		
			if (__this->has_texture) {
				gl2_surface_draw_shader_sampler_location(__this);
			
			}
		
			glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_BYTE, __this->faces);
		#endif
		
		return 0;
		
	}
	
#endif
