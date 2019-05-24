
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
		#if !KOS_USES_SHADER_PIPELINE
			signed long long* batch_command     = (signed long long*)            data;
			kos_gl_batch_device_batch_t* object = (kos_gl_batch_device_batch_t*) batch_command[1];
			
			if (batch_command[0] == 'p') { // draw
				glDisable(GL_CULL_FACE);
				
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
				
			} else if (batch_command[0] == 'i') { // add IVX model
				ivx_t* model = (ivx_t*) batch_command[10];
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
				
			} else if (batch_command[0] == 'a') { // add
				unsigned long long old_vertex_count = object->vertex_count; // get the current counts of the batch
				unsigned long long old_index_count  = object->index_count;
				
				if (object->vertex_count == 0) { // if nothing yet in the batch, simply malloc
					object->vertex_count += batch_command[3];
					object-> index_count += batch_command[9] * 3;
					
					object->positions      = (kos_gl_batch_device_batch_position_t*)      malloc(object->vertex_count * sizeof(kos_gl_batch_device_batch_position_t));
					object->colours        = (kos_gl_batch_device_batch_colour_t*)        malloc(object->vertex_count * sizeof(kos_gl_batch_device_batch_colour_t));
					object->texture_coords = (kos_gl_batch_device_batch_texture_coord_t*) malloc(object->vertex_count * sizeof(kos_gl_batch_device_batch_texture_coord_t));
					
					object->indices        = (uint32_t*)                                  malloc(object->index_count  * sizeof(uint32_t));
					
				} else { // if adding ontop of existing objects, realloc
					object->vertex_count += batch_command[3];
					object-> index_count += batch_command[9] * 3;
					
					object->positions      = (kos_gl_batch_device_batch_position_t*)      realloc(object->positions,      object->vertex_count * sizeof(kos_gl_batch_device_batch_position_t));
					object->colours        = (kos_gl_batch_device_batch_colour_t*)        realloc(object->colours,        object->vertex_count * sizeof(kos_gl_batch_device_batch_colour_t));
					object->texture_coords = (kos_gl_batch_device_batch_texture_coord_t*) realloc(object->texture_coords, object->vertex_count * sizeof(kos_gl_batch_device_batch_texture_coord_t));
					
					object->indices        = (uint32_t*)                                  realloc(object->indices,        object->index_count  * sizeof(uint32_t));
					
				}
				
				// add to batch
				
				kos_gl_device_vertex_line_face_t* faces = (kos_gl_device_vertex_line_face_t*) batch_command[8];
				
				kos_gl_device_vertex_t*        positions      = (kos_gl_device_vertex_t*)        batch_command[2];
				kos_gl_device_colour_t*        colours        = (kos_gl_device_colour_t*)        batch_command[4];
				kos_gl_device_texture_coord_t* texture_coords = (kos_gl_device_texture_coord_t*) batch_command[6];
				
				object->has_texture = batch_command[7] ? 1 : 0;
				
				unsigned long long i;
				for (i = old_vertex_count; i < object->vertex_count; i++) { // loop through each vertex of added object
					unsigned long long absolute = i - old_vertex_count;
					
					object->positions[i].x = (GLfloat) positions[absolute].x / FLOAT_ONE / 2;
					object->positions[i].y = (GLfloat) positions[absolute].y / FLOAT_ONE / 2;
					object->positions[i].z = (GLfloat) positions[absolute].z / FLOAT_ONE / 2;
					
					if (batch_command[5]) {
						object->colours[i].r = (GLfloat) colours[absolute].r / FLOAT_ONE;
						object->colours[i].g = (GLfloat) colours[absolute].g / FLOAT_ONE;
						object->colours[i].b = (GLfloat) colours[absolute].b / FLOAT_ONE;
						object->colours[i].a = (GLfloat) colours[absolute].a / FLOAT_ONE;
						
					} else {
						object->colours[i].r = 1.0f;
						object->colours[i].g = 1.0f;
						object->colours[i].b = 1.0f;
						object->colours[i].a = 1.0f;
						
					}
					
					object->texture_coords[i].x = 0.0f;
					object->texture_coords[i].y = 0.0f;
					
				} for (i = old_index_count / 3; i < object->index_count / 3; i++) {
					unsigned long long absolute = i - old_index_count;
					
					if (object->has_texture) {
						for (unsigned long long k = 0; k < 3; k++) {
							unsigned long long index = absolute;
							
							unsigned long long j;
							for (j = 0; j < batch_command[3]; j++) { // find texture couple in face list (loop through vertices)
								if      (k == 0 && faces[absolute].pair1[0] == j) { index = faces[absolute].pair1[1]; break; }
								else if (k == 1 && faces[absolute].pair2[0] == j) { index = faces[absolute].pair2[1]; break; }
								else if (k == 2 && faces[absolute].pair3[0] == j) { index = faces[absolute].pair3[1]; break; }
								
							}
							
							object->texture_coords[old_vertex_count + j].x = (GLfloat) texture_coords[index].x / FLOAT_ONE;
							object->texture_coords[old_vertex_count + j].y = (GLfloat) texture_coords[index].y / FLOAT_ONE;
							
						}
						
					}
					
					object->indices[i * 3    ] = faces[absolute].pair1[0];
					object->indices[i * 3 + 1] = faces[absolute].pair2[0];
					object->indices[i * 3 + 2] = faces[absolute].pair3[0];
					
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
		#else
			KOS_DEVICE_COMMAND_WARNING("gl batch");
		#endif
		
	}
	
#endif
