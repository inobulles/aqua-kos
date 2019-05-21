
#ifndef __AQUA__KOS_DEVICES_TEXTURE_H
	#define __AQUA__KOS_DEVICES_TEXTURE_H
	
	static void texture_device_handle(unsigned long long** result, const char* data) {
		if (strncmp(data, "sharp ", 6) == 0) SHARP_TEXTURES = atoi(data + 6);
		else KOS_DEVICE_COMMAND_WARNING("texture")
		
	}
	
#endif
