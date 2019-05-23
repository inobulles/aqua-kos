
#ifndef __AQUA__KOS_DEVICES_REQUESTS_H
	#define __AQUA__KOS_DEVICES_REQUESTS_H
	
	#include "../functions/requests.h"
	
	typedef struct {
		kos_request_response_t request_response;
		unsigned long long     pointer_to_const_url;
		
	} request_device_struct_t;
	
	static void requests_device_handle(unsigned long long** result, const char* data) {
		request_device_struct_t* request_device_struct = (request_device_struct_t*) data + 8;
		
		if      (strcmp(data, "get") == 0) kos_requests_get (&request_device_struct->request_response, (const char*) request_device_struct->pointer_to_const_url);
		else if (strcmp(data, "fre") == 0) kos_requests_free(&request_device_struct->request_response);
		else KOS_DEVICE_COMMAND_WARNING("requests")
		
	}
	
#endif
