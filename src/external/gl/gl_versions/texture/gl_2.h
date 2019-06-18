
#ifndef __AQUA__SDL2_SRC_KOS_GL_VERSIONS_TEXTURE_GL_2_H
	#define __AQUA__SDL2_SRC_KOS_GL_VERSIONS_TEXTURE_GL_2_H

	static void gl2_texture_parameters(void) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, TEXTURE_WRAP_TYPE); // x axis
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, TEXTURE_WRAP_TYPE); // y axis
		
		if (SHARP_TEXTURES) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			
		} else {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			
		}
		
	}
	
	texture_t gl2_texture_create(unsigned long long* _data, unsigned long long bpp, unsigned long long width, unsigned long long height) {
		unsigned char alpha = (unsigned char) (bpp % 32);
		
		unsigned long long* data = _data;
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		
		if (bpp > 32) { // assuming > 32 BPPs are unsupported by the HW ...
			unsigned long long data_bytes = width * height * (bpp >> 4);
			data = (unsigned long long*) malloc(data_bytes);
			
			unsigned long long i;
			for (i = 0; i < data_bytes; i++) {
				((uint8_t*) data)[i] = (uint8_t) (((uint16_t*) _data)[i] / 0x100);
				
			}
			
		}
		
		GLuint texture_id;
		glGenTextures(1, &texture_id);
		
		glBindTexture(GL_TEXTURE_2D, (GLuint) texture_id);
		glTexImage2D (GL_TEXTURE_2D, 0, alpha ? GL_RGB8 : GL_RGBA8, (GLuint) width, (GLuint) height, 0, alpha ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, data);
		
		if (bpp > 32) {
			free(data);
			
		}
		
		gl_texture_parameters();
		return texture_id;
		
	}
	
	void gl2_texture_remove(texture_t __this) {
		glDeleteTextures(1, (GLuint*) &__this);
		
	}
	
#endif
