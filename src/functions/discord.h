
#ifndef __AQUA__SDL2_KOS_FUNCTIONS_DISCORD_H
	#define __AQUA__SDL2_KOS_FUNCTIONS_DISCORD_H
	
	// following the Discord RPC documentation (https://discordapp.com/developers/docs/rich-presence/how-to)
	// and the "send presence" example         (https://github.com/discordapp/discord-rpc/blob/master/examples/send-presence/send-presence.c)
	
	#include "../discord/discord_rpc.h"
	
	typedef struct {
		// input
		
		unsigned long long state;
		unsigned long long details;
		
		unsigned long long large_image_text;
		unsigned long long small_image_text;
		
		unsigned long long large_image;
		unsigned long long small_image;
		
		unsigned long long party;
		
		unsigned long long party_size;
		unsigned long long party_max;
		
		// output
		
		unsigned long long has_connection;
		
		char username     [sizeof(unsigned long long) * 128];
		char discriminator[sizeof(unsigned long long) * 4];
		
	} kos_discord_rpc_t;
	
	static char*                kos_discord_rpc_application_id;
	static int64_t              kos_discord_rpc_start_time;
	static DiscordEventHandlers kos_discord_rpc_handlers;
	static kos_discord_rpc_t*   kos_latest_rpc;
	static unsigned char        kos_discord_rpc_session_active = 0;
	
	static void discord_rpc_generic_handler() {
		printf("TODO `%s`\n", __func__);
		
	}
	
	static void discord_rpc_ready_handler(const DiscordUser* user) {
		printf("INFO Discord RPC connected to user %s#%s - %s\n", user->username, user->discriminator, user->userId);
		kos_latest_rpc->has_connection = 1;
		
		strcpy(kos_latest_rpc->username,      user->username);
		strcpy(kos_latest_rpc->discriminator, user->discriminator);
		
	}
	
	#include <time.h>
	
	void update_discord_rpc(kos_discord_rpc_t* this) {
		DiscordRichPresence discord_presence;
		memset(&discord_presence, 0, sizeof(DiscordRichPresence));
		
		//~ discord_presence.startTimestamp = kos_discord_rpc_start_time;
		//~ discord_presence.endTimestamp   = time(0);
		
		discord_presence.state          = (const char*) this->state;
		discord_presence.details        = (const char*) this->details;
		
		discord_presence.largeImageText = (const char*) this->large_image_text;
		discord_presence.smallImageText = (const char*) this->small_image_text;
		
		discord_presence.largeImageKey  = (const char*) this->large_image;
		discord_presence.smallImageKey  = (const char*) this->small_image;
		
		discord_presence.partyId        = (const char*) this->party;
		discord_presence.partySize      = (int)         this->party_size;
		discord_presence.partyMax       = (int)         this->party_max;
		
		discord_presence.instance = 0;
		Discord_UpdatePresence(&discord_presence);
		
	}
	
	void loop_discord_rpc(kos_discord_rpc_t* this) {
		#ifdef DISCORD_DISABLE_IO_THREAD
			Discord_UpdateConnection();
		#endif
		
		kos_latest_rpc = this;
		Discord_RunCallbacks();
		
	}
	
	void dispose_discord_rpc(void);
	
	void init_discord_rpc(unsigned long long app_key) {
		if (kos_discord_rpc_session_active) {
			dispose_discord_rpc();
			
		}
		
		kos_discord_rpc_session_active = 1;
		kos_discord_rpc_application_id = (char*) app_key;
		
		kos_discord_rpc_start_time = time(0);
		memset(&kos_discord_rpc_handlers, 0, sizeof(DiscordEventHandlers));
		
		kos_discord_rpc_handlers.ready        = discord_rpc_ready_handler;
		kos_discord_rpc_handlers.disconnected = discord_rpc_generic_handler;
		kos_discord_rpc_handlers.errored      = discord_rpc_generic_handler;
		kos_discord_rpc_handlers.joinGame     = discord_rpc_generic_handler;
		kos_discord_rpc_handlers.spectateGame = discord_rpc_generic_handler;
		kos_discord_rpc_handlers.joinRequest  = discord_rpc_generic_handler;
		
		Discord_Initialize(kos_discord_rpc_application_id, &kos_discord_rpc_handlers, 1, NULL);
		
	}
	
	void dispose_discord_rpc(void) {
		if (kos_discord_rpc_session_active) {
			kos_discord_rpc_session_active = 0;
			Discord_Shutdown();
			
		}
		
	}
	
#endif
