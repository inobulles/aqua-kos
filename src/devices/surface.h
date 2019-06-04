
#ifndef __AQUA__KOS__DEVICES_SURFACE_H
	#define __AQUA__KOS__DEVICES_SURFACE_H
	
	#include "../external/gl/gl_common/surface.h"
	
	typedef struct {
		unsigned long long command;
		unsigned long long self;
		signed   long long args[4];
		
	} surface_device_t;
	
	void surface_device_handle(unsigned long long** result, const char* data) {
		surface_device_t* command = (surface_device_t*) data;
		
		if      (command->command == 'n') surface_new        (command->self, command->args[0], command->args[1], command->args[2], command->args[3]);
		else if (command->command == 'd') surface_draw       (command->self);
		else if (command->command == 't') surface_set_texture(command->self, command->args[0]);
		
		else KOS_DEVICE_COMMAND_WARNING("surface")
		
	}
	
#endif
