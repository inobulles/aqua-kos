
#ifndef __AQUA__KOS_DEVICES_JPG_H
	#define __AQUA__KOS_DEVICES_JPG_H
	
	#define STB_IMAGE_IMPLEMENTATION
	#include "../external/libstb/stb_image.h"
	
	static void jpg_device_handle(unsigned long long** result, const char* __data) {
		char* data = (char*) __data;
		if (!data) {
			*result = (unsigned long long*) 0;
			return;
			
		}
		
		int channels;
		stbi_set_flip_vertically_on_load(0);
		unsigned char* image = stbi_load_from_memory(data, 0x7FFFFFFF, (int*) &kos_bda_implementation.temp_value_field[1], (int*) &kos_bda_implementation.temp_value_field[2], &channels, STBI_rgb);
		
		kos_bda_implementation.temp_value_field[0] = texture_create((unsigned long long) image, channels * 8, kos_bda_implementation.temp_value_field[1], kos_bda_implementation.temp_value_field[2]);
		*result = (unsigned long long*) kos_bda_implementation.temp_value_field;
		stbi_image_free(image);
		
	}
	
#endif
