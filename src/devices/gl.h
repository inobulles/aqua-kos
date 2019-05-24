
#ifndef __AQUA__KOS_DEVICES_GL_H
	#define __AQUA__KOS_DEVICES_GL_H
	
	static void gl_device_handle(unsigned long long** result, const char* data) {
		const signed long long* gl_command = (const signed long long*) data;
			
		#if !KOS_USES_SHADER_PIPELINE
			/*if (gl_command[0] == 'b') { // batch commands
				if (gl_command[25] == 'e') { // draw elements
					kos_gl_device_vertex_line_face_t* faces = (kos_gl_device_vertex_line_face_t*) gl_command[17];
					unsigned long long                count = (unsigned long long)                gl_command[18];

					uint32_t* int_indices = (uint32_t*) malloc(count * 3 * sizeof(uint32_t));

					int i;
					for (i = 0; i < count; i++) {
						int_indices[i * 3    ] = faces[i].pair1[0];
						int_indices[i * 3 + 1] = faces[i].pair2[0];
						int_indices[i * 3 + 2] = faces[i].pair3[0];

					}

					glDrawElements(GL_TRIANGLES, count * 3, GL_UNSIGNED_INT, int_indices);
					free(int_indices);

				} else if (gl_command[25] == 's') { // enable/disable client state
					void (*client_state_function)(GLenum cap) = gl_command[10] ? glEnableClientState : glDisableClientState;

					if (gl_command[7]) client_state_function(GL_VERTEX_ARRAY);
					if (gl_command[8]) client_state_function(GL_COLOR_ARRAY);
					if (gl_command[9]) client_state_function(GL_TEXTURE_COORD_ARRAY);

				} else if (gl_command[25] == 'p') { // set pointer
					if (gl_command[7]) {
						unsigned long long count = gl_command[12];
						GLfloat* pointer = (GLfloat*) malloc(count * 3 * sizeof(GLfloat));
						kos_gl_device_vertex_t* vertices = (kos_gl_device_vertex_t*) gl_command[11];

						int i;
						for (i = 0; i < count; i++) {
							pointer[i * 3    ] = (double) vertices[i].x / FLOAT_ONE;
							pointer[i * 3 + 1] = (double) vertices[i].y / FLOAT_ONE;
							pointer[i * 3 + 2] = (double) vertices[i].z / FLOAT_ONE;

						}

						glVertexPointer(3, GL_FLOAT, 0, pointer);
						free(pointer);

					} if (gl_command[8]) {
						unsigned long long count = gl_command[20];
						GLfloat* pointer = (GLfloat*) malloc(count * 4 * sizeof(GLfloat));
						kos_gl_device_colour_t* colours = (kos_gl_device_colour_t*) gl_command[19];

						int i;
						for (i = 0; i < count; i++) {
							pointer[i * 4    ] = (double) colours[i].red   / FLOAT_ONE;
							pointer[i * 4 + 1] = (double) colours[i].green / FLOAT_ONE;
							pointer[i * 4 + 2] = (double) colours[i].blue  / FLOAT_ONE;
							pointer[i * 4 + 3] = (double) colours[i].alpha / FLOAT_ONE;

						}

						glColorPointer(4, GL_FLOAT, 0, pointer);
						free(pointer);

					} if (gl_command[9]) {
						unsigned long long count = gl_command[22];
						GLfloat* pointer = (GLfloat*) malloc(count * 2 * sizeof(GLfloat));
						kos_gl_device_texture_coord_t* texture_coords = (kos_gl_device_texture_coord_t*) gl_command[21];

						int i;
						for (i = 0; i < count; i++) {
							pointer[i * 2    ] = (double) texture_coords[i].x / FLOAT_ONE;
							pointer[i * 2 + 1] = (double) texture_coords[i].y / FLOAT_ONE;

						}

						glTexCoordPointer(2, GL_FLOAT, 0, pointer);
						free(pointer);

					}

				}

			} else */if (gl_command[0] == 't') { // bind/activate texture
				glActiveTexture(GL_TEXTURE0  + gl_command[23]);
				glBindTexture  (GL_TEXTURE_2D, gl_command[24]);
				
			} else if (gl_command[0] == 'f') { // frustum
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();

				glFrustum( \
					(double) gl_command[1] / FLOAT_ONE, \
					(double) gl_command[2] / FLOAT_ONE, \
					(double) gl_command[3] / FLOAT_ONE, \
					(double) gl_command[4] / FLOAT_ONE, \
					(double) gl_command[5] / FLOAT_ONE, \
					(double) gl_command[6] / FLOAT_ONE  \
				);
				
				glMatrixMode(GL_MODELVIEW);
				glLoadIdentity();
				
				glTranslatef(0.0f, 0.0f, -3.0f);
				
			} else if (gl_command[0] == 'o') { // ortho
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();

				glEnable(GL_CULL_FACE);
				glCullFace(GL_BACK);
				glFrontFace(GL_CCW);

				glOrtho( \
					(double) gl_command[1] / FLOAT_ONE, \
					(double) gl_command[2] / FLOAT_ONE, \
					(double) gl_command[3] / FLOAT_ONE, \
					(double) gl_command[4] / FLOAT_ONE, \
					(double) gl_command[5] / FLOAT_ONE, \
					(double) gl_command[6] / FLOAT_ONE  \
				);

				glTranslatef(0.0f, 0.0f, -100.0f);

				glMatrixMode(GL_MODELVIEW);
				glLoadIdentity();

			} else if (gl_command[0] == 'c') { // colour
				glColor4f( \
					(double) gl_command[13] / FLOAT_ONE, \
					(double) gl_command[14] / FLOAT_ONE, \
					(double) gl_command[15] / FLOAT_ONE, \
					(double) gl_command[16] / FLOAT_ONE  \
				);

			} else if (gl_command[0] == 'v') { // draw
				surface_t test;
				
				surface_new        ((unsigned long long) &test, -_UI64_MAX_MARGIN, -_UI64_MAX_MARGIN, 0, 0);
				surface_set_texture((unsigned long long) &test, 0);
				surface_draw       ((unsigned long long) &test);

				glPointSize(4.0f);
				glBegin(GL_POINTS);

				kos_gl_device_vertex_t*           vertices = (kos_gl_device_vertex_t*)           gl_command[11];
				kos_gl_device_vertex_line_face_t* faces    = (kos_gl_device_vertex_line_face_t*) gl_command[17];

				unsigned long long i;
				for (i = 0; i < gl_command[12]; i++) {
					glVertex3f((double) vertices[i].x / FLOAT_ONE, -(double) vertices[i].y / FLOAT_ONE, (double) vertices[i].z / FLOAT_ONE);

				}

				glEnd();
				glBegin(GL_LINES);

				for (i = 0; i < gl_command[18]; i++) {
					glVertex3f((double) vertices[faces[i].pair1[0]].x / FLOAT_ONE, -(double) vertices[faces[i].pair1[0]].y / FLOAT_ONE, (double) vertices[faces[i].pair1[0]].z / FLOAT_ONE);
					glVertex3f((double) vertices[faces[i].pair2[0]].x / FLOAT_ONE, -(double) vertices[faces[i].pair2[0]].y / FLOAT_ONE, (double) vertices[faces[i].pair2[0]].z / FLOAT_ONE);

					glVertex3f((double) vertices[faces[i].pair2[0]].x / FLOAT_ONE, -(double) vertices[faces[i].pair2[0]].y / FLOAT_ONE, (double) vertices[faces[i].pair2[0]].z / FLOAT_ONE);
					glVertex3f((double) vertices[faces[i].pair3[0]].x / FLOAT_ONE, -(double) vertices[faces[i].pair3[0]].y / FLOAT_ONE, (double) vertices[faces[i].pair3[0]].z / FLOAT_ONE);

					glVertex3f((double) vertices[faces[i].pair3[0]].x / FLOAT_ONE, -(double) vertices[faces[i].pair3[0]].y / FLOAT_ONE, (double) vertices[faces[i].pair3[0]].z / FLOAT_ONE);

					if (faces[i].pair4[0]) {
						glVertex3f((double) vertices[faces[i].pair4[0]].x / FLOAT_ONE, -(double) vertices[faces[i].pair4[0]].y / FLOAT_ONE, (double) vertices[faces[i].pair4[0]].z / FLOAT_ONE);
						glVertex3f((double) vertices[faces[i].pair4[0]].x / FLOAT_ONE, -(double) vertices[faces[i].pair4[0]].y / FLOAT_ONE, (double) vertices[faces[i].pair4[0]].z / FLOAT_ONE);

					}

					glVertex3f((double) vertices[faces[i].pair1[0]].x / FLOAT_ONE, -(double) vertices[faces[i].pair1[0]].y / FLOAT_ONE, (double) vertices[faces[i].pair1[0]].z / FLOAT_ONE);

				}

				glEnd();


			} else if (gl_command[0] == 'i') { // load identity
				glLoadIdentity();
				
			} else if (gl_command[0] == 'r') { // rotate
				glRotatef( \
					(double) gl_command[7]  / FLOAT_ONE, \
					(double) gl_command[8]  / FLOAT_ONE, \
					(double) gl_command[9]  / FLOAT_ONE, \
					(double) gl_command[10] / FLOAT_ONE  \
				);

			} else if (gl_command[0] == 'm') { // translate (move)
				glTranslatef( \
					(double) gl_command[7] / FLOAT_ONE, \
					(double) gl_command[8] / FLOAT_ONE, \
					(double) gl_command[9] / FLOAT_ONE  \
				);
				
			} else {
				KOS_DEVICE_COMMAND_WARNING("gl");

			}
		#else
			KOS_DEVICE_COMMAND_WARNING("gl");
		#endif
		
	}
	
#endif
