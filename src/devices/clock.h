
#ifndef __AQUA__KOS_DEVICES_TIME_H
	#define __AQUA__KOS_DEVICES_TIME_H
	
	#include <time.h>
	
	static struct tm* kos_tm_struct = (struct tm*) 0;
	static time_t     kos_time      = 0;
	
	static void clock_device_handle(unsigned long long** result, const char* data) {
		kos_time = time(NULL);
		kos_tm_struct = localtime(&kos_time);
		
		if (strcmp(data, "current") == 0) {
			kos_bda_implementation.temp_time_device.hour     = (uint64_t) kos_tm_struct->tm_hour;
			kos_bda_implementation.temp_time_device.minute   = (uint64_t) kos_tm_struct->tm_min;
			kos_bda_implementation.temp_time_device.second   = (uint64_t) kos_tm_struct->tm_sec;
			
			kos_bda_implementation.temp_time_device.day      = (uint64_t) kos_tm_struct->tm_mday;
			kos_bda_implementation.temp_time_device.month    = (uint64_t) kos_tm_struct->tm_mon;
			kos_bda_implementation.temp_time_device.year     = (uint64_t) kos_tm_struct->tm_year;
			
			kos_bda_implementation.temp_time_device.week_day = (uint64_t) kos_tm_struct->tm_wday;
			kos_bda_implementation.temp_time_device.year_day = (uint64_t) kos_tm_struct->tm_yday;
			
			*result = (unsigned long long*) &kos_bda_implementation.temp_time_device;
			
		} else {
			KOS_DEVICE_COMMAND_WARNING("clock")
			
		}
		
	}
	
#endif
