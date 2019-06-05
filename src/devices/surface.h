
#ifndef __AQUA__KOS__DEVICES_SURFACE_H
	#define __AQUA__KOS__DEVICES_SURFACE_H
	
	#include "../external/gl/gl_common/surface.h" /// TODO rewrite 'cause it's kinda a mess ATM
	
	void surface_device_handle(unsigned long long** result, const char* data) {
		unsigned long long* command = (unsigned long long*) data;
		
		if      (command[0] == 'n') surface_new        (command[1], command[2], command[3], command[4], command[5]);
		else if (command[0] == 'd') surface_draw       (command[1]);
		else if (command[0] == 't') surface_set_texture(command[1], command[2]);
		else if (command[0] == 'y') surface_set_layer  (command[1], command[2]);
		else if (command[0] == 's') surface_scroll     (command[1], command[2], command[3], command[4], command[5]);
		
		else if (command[0] == 'z') {
			surface_set_width (command[1], command[2]);
			surface_set_height(command[1], command[3]);
			
		} else if (command[0] == 'p') {
			surface_set_x(command[1], command[2]);
			surface_set_y(command[1], command[3]);
			
		} else if (command[0] == 'c') {
			surface_set_colour(command[1], command[2], command[3], command[4]);
			surface_set_alpha (command[1], command[5]);
			
		}
		
		else KOS_DEVICE_COMMAND_WARNING("surface")
		
	}
	
#endif
