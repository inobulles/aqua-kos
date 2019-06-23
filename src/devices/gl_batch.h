
#ifndef __AQUA__KOS_DEVICES_GL_BATCH_H
	#define __AQUA__KOS_DEVICES_GL_BATCH_H

	int glGetAttribLocation       ();
	int glEnableVertexAttribArray ();
	int glBindBuffer              ();
	int glVertexAttribPointer     ();
	int glDisableVertexAttribArray();
	int glBufferData              ();
	int glGenBuffers              ();
	
	typedef struct {
		unsigned long long version_major;
		unsigned long long version_minor;
		unsigned long long is_index_size_short;
		char name[1024];
		
		unsigned long long vertex_count;
		unsigned long long coords_count;
		unsigned long long  index_count;
		
		unsigned long long vertex_bytes;
		unsigned long long coords_bytes;
		unsigned long long  index_bytes;
		
	} ivx_meta_t;
	
	typedef struct {
		ivx_meta_t meta;
		
		unsigned long long vertex_pointer;
		unsigned long long coords_pointer;
		unsigned long long  index_pointer;
		
	} ivx_t;
	
	typedef struct {
		signed long long x;
		signed long long y;
		signed long long z;
		
	} kos_gl_device_vertex_t;
	
	typedef struct { // quads
		unsigned long long pair1[2];
		unsigned long long pair2[2];
		unsigned long long pair3[2];
		unsigned long long pair4[2];
		
	} kos_gl_device_vertex_line_face_t;
	
	typedef struct {
		unsigned long long r;
		unsigned long long g;
		unsigned long long b;
		unsigned long long a;
		
	} kos_gl_device_colour_t;
	
	typedef struct {
		unsigned long long x;
		unsigned long long y;
		
	} kos_gl_device_texture_coord_t;
	
	typedef struct { GLfloat x; GLfloat y; GLfloat z;            } __attribute__((packed)) kos_gl_batch_device_batch_position_t;
	typedef struct { GLfloat r; GLfloat g; GLfloat b; GLfloat a; } __attribute__((packed)) kos_gl_batch_device_batch_colour_t;
	typedef struct { GLfloat x; GLfloat y;                       } __attribute__((packed)) kos_gl_batch_device_batch_texture_coord_t;
	
	typedef struct {
		unsigned char has_texture;
		
		unsigned long long vertex_count;
		kos_gl_batch_device_batch_position_t* positions;
		
		unsigned long long colour_count;
		kos_gl_batch_device_batch_colour_t* colours;
		
		unsigned long long coords_count;
		kos_gl_batch_device_batch_texture_coord_t* texture_coords;
		
		unsigned long long index_count;
		uint32_t*          indices;
		
		//~ GLuint vertex_buffer;
		//~ GLuint coords_buffer;
		//~ GLuint colour_buffer;
		//~ GLuint  index_buffer;
		
	} kos_gl_batch_device_batch_t;
	
	static void gl_batch_device_handle(unsigned long long** result, const char* data) {
		signed long long* batch_command     = (signed long long*)            data;
		kos_gl_batch_device_batch_t* object = (kos_gl_batch_device_batch_t*) batch_command[1];
		
		if (batch_command[0] == 'p') { // draw
			glDisable(GL_CULL_FACE);
			
			#if !KOS_USES_SHADER_PIPELINE
				glEnableClientState(GL_VERTEX_ARRAY);
				glEnableClientState(GL_COLOR_ARRAY);
				
				if (object->has_texture) {
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					
				}
				
				glVertexPointer(3, GL_FLOAT, 0, object->positions);
				glColorPointer (4, GL_FLOAT, 0, object->colours);
				
				if (object->has_texture) {
					glTexCoordPointer(2, GL_FLOAT, 0, object->texture_coords);
					
				}
				
				glDrawElements(GL_TRIANGLES, object->index_count, GL_UNSIGNED_INT, object->indices);
				
				glDisableClientState(GL_VERTEX_ARRAY);
				glDisableClientState(GL_COLOR_ARRAY);
				
				if (object->has_texture) {
					glDisableClientState(GL_TEXTURE_COORD_ARRAY);
					
				}
			#else
				if (!shader_has_set_locations) {
					printf("WARNING Shader has not yet set locations\n");
					
				} else {
					ESMatrix mvp_matrix;
					esMatrixMultiply(&mvp_matrix, &model_view_matrix, &projection_matrix);
					glUniformMatrix4fv(shader_mvp_matrix_location, 1, GL_FALSE, &mvp_matrix.m[0][0]);
					
				}
			
				vertex_attribute_pointer(0, 3, object->positions);
				vertex_attribute_pointer(1, 2, object->texture_coords);
				vertex_attribute_pointer(2, 4, object->colours);
				
				glEnableVertexAttribArray(0);
				glEnableVertexAttribArray(1);
				glEnableVertexAttribArray(2);
			
				if (object->has_texture) {
					glUniform1i(shader_has_texture_location, 1);
					glUniform1i(shader_sampler_location, 0);
					
				}
				
				glDrawElements(GL_TRIANGLES, (GLsizei) object->index_count, GL_UNSIGNED_INT, object->indices);
			#endif
			
		} else if (batch_command[0] == 'i') { // add IVX model
			ivx_t* model = (ivx_t*) batch_command[2];
			object->has_texture   = 1;//batch_command[7] ? 1 : 0;
			
			unsigned long long old_vertex_count = object->vertex_count; // get the current counts of the batch
			unsigned long long old_coords_count = object->coords_count;
			unsigned long long old_index_count  = object->index_count;
			
			object->vertex_count += model->meta.vertex_count;
			object->coords_count += model->meta.coords_count;
			object-> index_count += model->meta. index_count;
			
			if (old_vertex_count == 0) { // if nothing yet in batch, simply malloc
				object->positions      = (kos_gl_batch_device_batch_position_t*)      malloc(object->vertex_count * sizeof(kos_gl_batch_device_batch_position_t));
				object->colours        = (kos_gl_batch_device_batch_colour_t*)        malloc(object->vertex_count * sizeof(kos_gl_batch_device_batch_colour_t));
				object->texture_coords = (kos_gl_batch_device_batch_texture_coord_t*) malloc(object->coords_count * sizeof(kos_gl_batch_device_batch_texture_coord_t));
				object->indices        = (uint32_t*)                                  malloc(object-> index_count * sizeof(uint32_t));
				
			} else { // if adding ontop of existing objects, realloc
				object->positions      = (kos_gl_batch_device_batch_position_t*)      realloc(object->positions,      object->vertex_count * sizeof(kos_gl_batch_device_batch_position_t));
				object->colours        = (kos_gl_batch_device_batch_colour_t*)        realloc(object->colours,        object->vertex_count * sizeof(kos_gl_batch_device_batch_colour_t));
				object->texture_coords = (kos_gl_batch_device_batch_texture_coord_t*) realloc(object->texture_coords, object->coords_count * sizeof(kos_gl_batch_device_batch_texture_coord_t));
				object->indices        = (uint32_t*)                                  realloc(object->indices,        object-> index_count * sizeof(uint32_t));
				
			}
			
			// add to batch
			
			for (unsigned long long i = old_vertex_count; i < object->vertex_count; i++) {
				object->colours[i].r = 1.0f;
				object->colours[i].g = 1.0f;
				object->colours[i].b = 1.0f;
				object->colours[i].a = 1.0f;
				
			}
			
			memcpy(object->positions      + old_vertex_count * sizeof(kos_gl_batch_device_batch_position_t),      (void*) model->vertex_pointer, model->meta.vertex_count * sizeof(kos_gl_batch_device_batch_position_t));
			memcpy(object->texture_coords + old_coords_count * sizeof(kos_gl_batch_device_batch_texture_coord_t), (void*) model->coords_pointer, model->meta.coords_count * sizeof(kos_gl_batch_device_batch_texture_coord_t));
			memcpy(object->indices        +  old_index_count * sizeof(object->indices[0]),                        (void*) model->index_pointer,  model->meta. index_count * sizeof(object->indices[0]));
			
		} else if (batch_command[0] == 'u') { // colour
			for (unsigned long long i = 0; i < object->vertex_count; i++) {
				object->colours[i].r = (float) ((unsigned long long) batch_command[2]) / _UI64_MAX;
				object->colours[i].g = (float) ((unsigned long long) batch_command[3]) / _UI64_MAX;
				object->colours[i].b = (float) ((unsigned long long) batch_command[4]) / _UI64_MAX;
				object->colours[i].a = (float) ((unsigned long long) batch_command[5]) / _UI64_MAX;
				
			}
			
		} else if (batch_command[0] == 'c') { // create
			object = (kos_gl_batch_device_batch_t*) malloc(sizeof(kos_gl_batch_device_batch_t));
			batch_command[1] = (signed long long) object;
			
			object->vertex_count = 0;
			object-> index_count = 0;
			object->coords_count = 0;
			object->has_texture  = 0;
			
		} else if (batch_command[0] == 'd') { // dispose
			if (object->vertex_count) {
				free(object->positions);
				free(object->colours);
				free(object->texture_coords);
				
			} if (object->index_count) {
				free(object->indices);
				
			}
			
			free(object);
			
		} else {
			KOS_DEVICE_COMMAND_WARNING("gl batch");
			
		}
		
	}
	
#endif
