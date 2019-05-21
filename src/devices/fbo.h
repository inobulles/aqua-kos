
#ifndef __AQUA__KOS_DEVICES_FBO_H
	#define __AQUA__KOS_DEVICES_FBO_H
	
	static void fbo_device_handle(unsigned long long** result, const char* data) {
		const unsigned long long* fbo_command = (const unsigned long long*) data;
		
		if (fbo_command[0] == 'c') { // create
			kos_bda_implementation.temp_value = framebuffer_create(fbo_command[1]);
			*result = (unsigned long long*) &kos_bda_implementation.temp_value;
			
		}
			
		else if (fbo_command[0] == 'b') framebuffer_bind  (fbo_command[1], fbo_command[4], fbo_command[5], fbo_command[2], fbo_command[3]);
		else if (fbo_command[0] == 'r') framebuffer_remove(fbo_command[1]);
		else KOS_DEVICE_COMMAND_WARNING("fbo");
		
	}
	
#endif
