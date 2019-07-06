
#ifndef __AQUA__KOS_DEVICES_TIME_H
	#define __AQUA__KOS_DEVICES_TIME_H
	
	#include <time.h>
	
	static struct tm* kos_tm_struct = (struct tm*) 0;
	static time_t     kos_time      = 0;
	
	static void clock_device_handle(unsigned long long** result, const char* data) {
		kos_time = time(NULL);
		kos_tm_struct = localtime(&kos_time);
		
		if (strcmp(data, "unix") == 0) {
			struct timespec now;
			clock_gettime(CLOCK_MONOTONIC, &now);
			kos_bda_implementation.temp_value = now.tv_sec * 1000000000ull + now.tv_nsec;
			
		} else if (strcmp(data, "current") == 0) {
			kos_bda_implementation.temp_value_field[0] = (unsigned long long) kos_tm_struct->tm_hour;
			kos_bda_implementation.temp_value_field[1] = (unsigned long long) kos_tm_struct->tm_min;
			kos_bda_implementation.temp_value_field[2] = (unsigned long long) kos_tm_struct->tm_sec;
			
			kos_bda_implementation.temp_value_field[3] = (unsigned long long) kos_tm_struct->tm_mday;
			kos_bda_implementation.temp_value_field[4] = (unsigned long long) kos_tm_struct->tm_mon;
			kos_bda_implementation.temp_value_field[5] = (unsigned long long) kos_tm_struct->tm_year;
			
			kos_bda_implementation.temp_value_field[6] = (unsigned long long) kos_tm_struct->tm_wday;
			kos_bda_implementation.temp_value_field[7] = (unsigned long long) kos_tm_struct->tm_yday;
			
			*result = (unsigned long long*) kos_bda_implementation.temp_value_field;
			
		} else {
			KOS_DEVICE_COMMAND_WARNING("clock")
			
		}
		
	}
	
#endif
