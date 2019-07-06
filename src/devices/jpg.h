
#ifndef __AQUA__KOS_DEVICES_JPG_H
	#define __AQUA__KOS_DEVICES_JPG_H
	
	#ifndef STB_IMAGE_IMPLEMENTATION
		#define STB_IMAGE_IMPLEMENTATION
		#include "../external/libstb/stb_image.h"
	#endif
	
	static void jpg_device_handle(unsigned long long** result, const char* data) {
		unsigned long long* command = (unsigned long long*) data;
		if (!command[0]) {
			*result = (unsigned long long*) 0;
			return;
			
		}
		
		stbi_set_flip_vertically_on_load(0);
		unsigned char* image = stbi_load_from_memory((const stbi_uc*) command[0], command[1], (int*) &kos_bda_implementation.temp_value_field[1], (int*) &kos_bda_implementation.temp_value_field[2], (int*) &kos_bda_implementation.temp_value_field[3], 0);
		
		kos_bda_implementation.temp_value_field[0] = texture_create((unsigned long long) image, kos_bda_implementation.temp_value_field[3] * 8, kos_bda_implementation.temp_value_field[1], kos_bda_implementation.temp_value_field[2]);
		*result = (unsigned long long*) kos_bda_implementation.temp_value_field;
		stbi_image_free(image);
		
	}
	
#endif
