
#ifndef __AQUA__KOS_DEVICES_SHADER_H
	#define __AQUA__KOS_DEVICES_SHADER_H
	
	#include "../external/gl/gl_common/shaders.h"
	
	static void shader_device_handle(unsigned long long** result, const char* data) {
		const unsigned long long* shader_command = (const unsigned long long*) data;
		
		if (shader_command[0] == 'c') { // create
			kos_bda_implementation.temp_value = gl_load_shaders((GLuint*) shader_command[1], (char*) shader_command[3], (char*) shader_command[4]);
			*result = (unsigned long long*) &kos_bda_implementation.temp_value;
			
		}
			
		else if (shader_command[0] == 'u') gl_use_shader_program   ((GLuint*) shader_command[1]);
		else if (shader_command[0] == 'r') gl_delete_shader_program((GLuint*) shader_command[1]);
		else KOS_DEVICE_COMMAND_WARNING("shader");
		
	}
	
#endif
