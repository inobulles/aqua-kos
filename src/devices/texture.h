
typedef unsigned long long texture_t;

static int SHARP_TEXTURES = 0;

static void gl_texture_parameters(void) {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	if (SHARP_TEXTURES) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		
	} else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
}

texture_t texture_create(unsigned long long _data, unsigned long long bpp, unsigned long long width, unsigned long long height) {
	unsigned char alpha = bpp % 32;
	unsigned long long* data = (unsigned long long*) _data;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	if (bpp > 32) { // assuming > 32 BPPs are unsupported by the HW ...
		unsigned long long data_bytes = width * height * (bpp >> 4);
		data = (unsigned long long*) malloc(data_bytes);
		
		for (unsigned long long i = 0; i < data_bytes; i++) {
			((char*) data)[i] = ((char*) _data)[i << 1];
		}
	}
	
	GLuint texture_id;
	glGenTextures(1, (GLuint*) &texture_id);
	
	glBindTexture(GL_TEXTURE_2D, (GLuint) texture_id);
	glTexImage2D (GL_TEXTURE_2D, 0, alpha ? GL_RGB : GL_RGBA, (GLuint) width, (GLuint) height, 0, alpha ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, data);
	
	if (bpp > 32) {
		free(data);
	}
	
	gl_texture_parameters();
	return texture_id;
}

void texture_remove(texture_t self) {
	glDeleteTextures(1, (GLuint*) &self);
}

static void texture_device_handle(unsigned long long** result, const char* data) {
	unsigned long long* command = (unsigned long long*) data;
	
	if      (command[0] == 'c') { kos_bda_implementation.temp_value = texture_create(command[1], command[2], command[3], command[4]); *result = (unsigned long long*) &kos_bda_implementation.temp_value; }
	else if (command[0] == 'r') texture_remove(command[1]);
	
	else if (strncmp(data, "sharp ", 6)  == 0) SHARP_TEXTURES = (unsigned long long) atoi(data + 6);
}
