
#ifndef __AQUA__KOS_DEVICES_DEBUG_H
	#define __AQUA__KOS_DEVICES_DEBUG_H
	
	void heap_memory_snapshot_start(void);
	void heap_memory_snapshot_end  (void);
	
	static void debug_device_handle(unsigned long long** result, const char* data) {
		if (strcmp(data, "mem.snap.start") == 0) {
			printf("=== debug.mem.snap.start ===\n"); /// TODO
			//~ heap_memory_snapshot_start();
			
		} else if (strcmp(data, "mem.snap.end") == 0) {
			printf("=== debug.mem.snap.end ===\n"); /// TODO
			//~ heap_memory_snapshot_end();
			
		} else {
			KOS_DEVICE_COMMAND_WARNING("debug")
			
		}
		
	}
	
#endif
