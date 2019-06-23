
#ifndef __AQUA__KOS_DEVICES_FRAMEBUFFER_H
	#define __AQUA__KOS_DEVICES_FRAMEBUFFER_H
	
	#include "../external/gl/gl_common/framebuffer.h"
	
	static void framebuffer_device_handle(unsigned long long** result, const char* data) {
		unsigned long long* framebuffer_command = (unsigned long long*) data;
		
		if (framebuffer_command[0] == 'c') { // create
			kos_bda_implementation.temp_value = framebuffer_create(framebuffer_command[1]);
			*result = (unsigned long long*) &kos_bda_implementation.temp_value;
			
		}
			
		else if (framebuffer_command[0] == 'b') framebuffer_bind  (framebuffer_command[1], framebuffer_command[4], framebuffer_command[5], framebuffer_command[2], framebuffer_command[3]);
		else if (framebuffer_command[0] == 'r') framebuffer_remove(framebuffer_command[1]);
		else KOS_DEVICE_COMMAND_WARNING("framebuffer");
		
	}
	
#endif
