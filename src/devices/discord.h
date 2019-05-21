
#ifndef __AQUA__KOS_DEVICES_DISCORD_H
	#define __AQUA__KOS_DEVICES_DISCORD_H
	
	#include "../fucntions/discord.h"
	
	typedef struct {
		kos_discord_rpc_t discord_rpc;
		
	} discord_device_struct_t;
	
	static void discord_device_handle(unsigned long long** result, const char* data) {
		discord_device_struct_t* discord_device_struct = (discord_device_struct_t*) data + 9;
		
		if      (strcmp(extra, "rpc init") == 0) init_discord_rpc  ((unsigned long long) data);
		else if (strcmp(extra, "rpc loop") == 0) loop_discord_rpc  (&discord_device_struct->discord_rpc);
		else if (strcmp(extra, "rpc updt") == 0) update_discord_rpc(&discord_device_struct->discord_rpc);
		else if (strcmp(extra, "rpc disp") == 0) dispose_discord_rpc();
		else KOS_DEVICE_COMMAND_WARNING("discord")
		
	}
	
#endif
