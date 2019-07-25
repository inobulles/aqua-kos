
#ifndef __AQUA__KOS_DEVICES_GL_H
	#define __AQUA__KOS_DEVICES_GL_H
	
	static void gl_device_handle(unsigned long long** result, const char* data) {
		const signed long long* gl_command = (const signed long long*) data;
		if (gl_command[0] == 't') { // bind/activate texture
			glActiveTexture((GLenum) (GL_TEXTURE0 + gl_command[23]));
			glBindTexture  (GL_TEXTURE_2D, (GLenum) gl_command[24]);
			
		} else if (gl_command[0] == 'f') { // frustum
			glDisable(GL_CULL_FACE);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();

			glFrustum( \
				(float) gl_command[1] / FLOAT_ONE, \
				(float) gl_command[2] / FLOAT_ONE, \
				(float) gl_command[3] / FLOAT_ONE, \
				(float) gl_command[4] / FLOAT_ONE, \
				(float) gl_command[5] / FLOAT_ONE, \
				(float) gl_command[6] / FLOAT_ONE  \
			);
			
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			
			glTranslatef(0.0f, 0.0f, 3.0f);
			
		} else if (gl_command[0] == 'o') { // ortho
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();

			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glFrontFace(GL_CCW);

			glOrtho( \
				(float) gl_command[1] / FLOAT_ONE, \
				(float) gl_command[2] / FLOAT_ONE, \
				(float) gl_command[3] / FLOAT_ONE, \
				(float) gl_command[4] / FLOAT_ONE, \
				(float) gl_command[5] / FLOAT_ONE, \
				(float) gl_command[6] / FLOAT_ONE  \
			);

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			
			glTranslatef(0.0f, 0.0f, -100.0f);

		} else if (gl_command[0] == 'c') { // colour
			#if !KOS_USES_SHADER_PIPELINE
				glColor4f( \
					(double) gl_command[13] / FLOAT_ONE, \
					(double) gl_command[14] / FLOAT_ONE, \
					(double) gl_command[15] / FLOAT_ONE, \
					(double) gl_command[16] / FLOAT_ONE  \
				);
			#else
				KOS_DEVICE_COMMAND_WARNING("gl")
			#endif

		} else if (gl_command[0] == 'v') { // draw
			#if !KOS_USES_SHADER_PIPELINE
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
			#else
				KOS_DEVICE_COMMAND_WARNING("gl")
			#endif

		} else if (gl_command[0] == 'i') { // load identity
			glLoadIdentity();
			
		} else if (gl_command[0] == 'h') { // depth
			if (gl_command[23]) glEnable (GL_DEPTH_TEST);
			else                glDisable(GL_DEPTH_TEST);
			
		} else if (gl_command[0] == 'r') { // rotate
			glRotatef( \
				(float) gl_command[7]  / FLOAT_ONE, \
				(float) gl_command[8]  / FLOAT_ONE, \
				(float) gl_command[9]  / FLOAT_ONE, \
				(float) gl_command[10] / FLOAT_ONE  \
			);

		} else if (gl_command[0] == 's') { // scale
			glScalef( \
				(float) gl_command[7]  / FLOAT_ONE, \
				(float) gl_command[8]  / FLOAT_ONE, \
				(float) gl_command[9]  / FLOAT_ONE  \
			);

		} else if (gl_command[0] == 'm') { // translate (move)
			glTranslatef( \
				(float) gl_command[7] / FLOAT_ONE, \
				(float) gl_command[8] / FLOAT_ONE, \
				(float) gl_command[9] / FLOAT_ONE  \
			);
			
		} else if (gl_command[0] == 'l') { // scaLe
			glScalef( \
				(float) gl_command[7] / FLOAT_ONE, \
				(float) gl_command[8] / FLOAT_ONE, \
				(float) gl_command[9] / FLOAT_ONE \
			);
			
		} else {
			KOS_DEVICE_COMMAND_WARNING("gl");

		}
		
	}
	
#endif
