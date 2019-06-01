
#ifndef __AQUA__KOS__DEVICES_SURFACE_H
	#define __AQUA__KOS__DEVICES_SURFACE_H
	
	#include "../external/gl/gl_common/surface.h"
	
	void surface_device_handle(unsigned long long** result, const char* data) {
		unsigned long long* command = (unsigned long long*) data;
		
		if      (command[0] == 'n') surface_new        (command[1], command[2], command[3], command[4], command[5]);
		else if (command[0] == 'd') surface_draw       (command[1]);
		else if (command[0] == 't') surface_set_texture(command[1], command[2]);
		
		else KOS_DEVICE_COMMAND_WARNING("surface")
		
	}
	
#endif
