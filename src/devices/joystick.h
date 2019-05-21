
#ifndef __AQUA__KOS_DEVICES_JOYSTICK_H
	#define __AQUA__KOS_DEVICES_JOYSTICK_H
	
	#include "../functions/joystick.h"
	
	static void joystick_device_handle(unsigned long long** result, const char* data) {
		unsigned char result_string = 0;
		
		if (strcmp(data, "name") == 0) {
			strncpy(kos_bda_implementation.temp_string, kos_get_joystick_name(kos_current_joystick), sizeof(kos_bda_implementation.temp_string));
			result_string = 1;
			
		} else if (strncmp(data, "select ", 7) == 0) {
			kos_select_joystick(atoi(data + 7));
			
		}
		
		else if (strcmp (data, "count"       ) == 0) kos_bda_implementation.temp_value = kos_joystick_count;
		else if (strcmp (data, "button count") == 0) kos_bda_implementation.temp_value = kos_joystick_button_count(kos_current_joystick);
		else if (strcmp (data, "axis count"  ) == 0) kos_bda_implementation.temp_value = kos_joystick_axis_count  (kos_current_joystick);
		else if (strcmp (data, "hat count"   ) == 0) kos_bda_implementation.temp_value = kos_joystick_hat_count   (kos_current_joystick);
		
		else if (strncmp(data, "button ",   7) == 0) kos_bda_implementation.temp_value =                       kos_get_joystick_button(kos_current_joystick, atoi(data + 7));
		else if (strncmp(data, "hat ",      4) == 0) kos_bda_implementation.temp_value =                       kos_get_joystick_hat   (kos_current_joystick, atoi(data + 4));
		else if (strncmp(data, "axis ",     5) == 0) kos_bda_implementation.temp_value = (unsigned long long) (kos_get_joystick_axis  (kos_current_joystick, atoi(data + 5)) * FLOAT_ONE);
		
		else KOS_DEVICE_COMMAND_WARNING("joystick")
		*result =  result_string ? (unsigned long long*) kos_bda_implementation.temp_string : &kos_bda_implementation.temp_value;
		
	}
	
#endif
