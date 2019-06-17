
#ifndef __AQUA__KOS_DEVICES_MATH_H
	#define __AQUA__KOS_DEVICES_MATH_H
	
	#include <math.h>

	typedef struct {
		char signature[sizeof(uint64_t)];
		int64_t x;
		int64_t y;
		
	} math_device_generic_t;
	
	double exponential(double x) {
		int precision = 10;
		double sum = 1.0f;
		
		for (int i = precision - 1; i > 0; i--) {
			sum = sum * x / i + 1;
			
		}
		
		return sum;
		
	}
	
	static void math_device_handle(unsigned long long** result, const char* data) {
		const math_device_generic_t* device = (const math_device_generic_t*) data;
		
		if      (strcmp(data, "sqrt"   ) == 0) kos_bda_implementation.temp_value = (unsigned long long) (sqrt ((double) device->x / FLOAT_ONE)                                 * FLOAT_ONE);
		else if (strcmp(data, "sin"    ) == 0) kos_bda_implementation.temp_value = (unsigned long long) (sin  ((double) device->x / FLOAT_ONE)                                 * FLOAT_ONE);
		else if (strcmp(data, "atan2"  ) == 0) kos_bda_implementation.temp_value = (unsigned long long) (atan2((double) device->x / FLOAT_ONE, (double) device->y / FLOAT_ONE) * FLOAT_ONE);
		else if (strcmp(data, "sigmoid") == 0) {
			double exp = exponential((double) ((int64_t) device->x) / FLOAT_ONE);
			kos_bda_implementation.temp_value = (unsigned long long) ((((exp / (exp + 1.0f)) - 0.5f) * 2.0f) * FLOAT_ONE);
			
		} else {
			KOS_DEVICE_COMMAND_WARNING("math")
			
		} *result = &kos_bda_implementation.temp_value;
		
	}
	
	
#endif
