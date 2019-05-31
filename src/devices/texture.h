
#ifndef __AQUA__KOS_DEVICES_TEXTURE_H
	#define __AQUA__KOS_DEVICES_TEXTURE_H
	
	#include "../external/gl/gl_common/texture.h"
	
	texture_t create_texture_from_screenshot(void) {
		#if KOS_USES_JNI
			return (texture_t) CALLBACK_INT(java_create_texture_from_screenshot, video_width(), video_height(), TEXTURE_WRAP_TYPE, SHARP_TEXTURES);
		#else
			unsigned long long bpp   = 32;
			unsigned long long bytes = video_width() * video_height() * (bpp >> 3);
			
			unsigned char* pixels = (unsigned char*) malloc(bytes);
			glReadBuffer(GL_FRONT);
			glReadPixels(0, 0, (GLsizei) video_width(), (GLsizei) video_height(), GL_RGBA, GL_UNSIGNED_BYTE, pixels);
			
			unsigned long long* data  = (unsigned long long*) malloc(bytes);
			unsigned char*      data8 = (unsigned char*)      data;
			unsigned long long  pitch = video_width() * (bpp / 8);
			
			int y;
			for (y = 0; y < video_height(); y++) {
				memcpy(data8 + (video_height() - y - 1) * pitch, pixels + y * pitch, pitch);
				
			}
			
			texture_t texture = texture_create((unsigned long long) data, bpp, video_width(), video_height());
			
			free(pixels);
			free(data);
			
			if (texture == -1ull) {
				printf("WARNING Could not create texture from screenshot\n");
				return 0;
				
			} else {
				return texture;
			}
		#endif
		
	}
	
	typedef struct {
		unsigned long long command;
		unsigned long long x, y, z, w;
		
	} texture_device_t;
	
	static void texture_device_handle(unsigned long long** result, const char* data) {
		texture_device_t* command = (texture_device_t*) data;
		
		if      (command->command == 'c') { kos_bda_implementation.temp_value = texture_create(command->x, command->y, command->z, command->w); *result = (unsigned long long*) &kos_bda_implementation.temp_value; }
		else if (command->command == 'r') texture_remove(command->x);
		
		else if (strcmp(data, "screenshot") == 0) { kos_bda_implementation.temp_value = create_texture_from_screenshot();                       *result = (unsigned long long*) &kos_bda_implementation.temp_value; }
		else if (strncmp(data, "sharp ", 6) == 0) SHARP_TEXTURES = atoi(data + 6);
		else KOS_DEVICE_COMMAND_WARNING("texture")
		
	}
	
#endif
