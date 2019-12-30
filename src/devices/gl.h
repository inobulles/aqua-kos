
static void gl_device_handle(unsigned long long** result, const char* data) {
	const signed long long* gl_command = (const signed long long*) data;
	
	if (gl_command[0] == 't') { // bind / activate texture
		glActiveTexture((GLenum) (GL_TEXTURE0 + gl_command[23]));
		glBindTexture(GL_TEXTURE_2D, (GLenum) gl_command[24]);
	}
	
	else if (gl_command[0] == 'f') { // frustum
		glDisable(GL_CULL_FACE);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		glFrustumf( \
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
	}
	
	else if (gl_command[0] == 'o') { // ortho
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);

		glOrthof( \
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
	}
	
	else if (gl_command[0] == 'c') { // colour
		glColor4f( \
			(double) gl_command[13] / FLOAT_ONE, \
			(double) gl_command[14] / FLOAT_ONE, \
			(double) gl_command[15] / FLOAT_ONE, \
			(double) gl_command[16] / FLOAT_ONE  \
		);
	}
	
	else if (gl_command[0] == 'i') { // load identity
		glLoadIdentity();
	}
	
	else if (gl_command[0] == 'h') { // depth
		if (gl_command[23]) glEnable (GL_DEPTH_TEST);
		else                glDisable(GL_DEPTH_TEST);
	}
	
	else if (gl_command[0] == 'r') { // rotate
		glRotatef( \
			(float) gl_command[7]  / FLOAT_ONE, \
			(float) gl_command[8]  / FLOAT_ONE, \
			(float) gl_command[9]  / FLOAT_ONE, \
			(float) gl_command[10] / FLOAT_ONE  \
		);
	}
	
	else if (gl_command[0] == 's') { // scale
		glScalef( \
			(float) gl_command[7]  / FLOAT_ONE, \
			(float) gl_command[8]  / FLOAT_ONE, \
			(float) gl_command[9]  / FLOAT_ONE  \
		);
	}
	
	else if (gl_command[0] == 'm') { // translate (move)
		glTranslatef( \
			(float) gl_command[7] / FLOAT_ONE, \
			(float) gl_command[8] / FLOAT_ONE, \
			(float) gl_command[9] / FLOAT_ONE  \
		);
	}
}
