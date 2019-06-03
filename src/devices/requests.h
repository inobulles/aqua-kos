
#ifndef __AQUA__KOS_DEVICES_REQUESTS_H
	#define __AQUA__KOS_DEVICES_REQUESTS_H
	
	typedef struct {
		char* address;
		
	} request_response_t;
	
	int asprintf(char **strp, const char *fmt, ...);
	
	#include "../external/librequests/include/requests.h"
	#define __OMIT_INCLUSION_OF_REQUESTS_HEADER__ 1
	#include "../external/librequests/src/requests.c"
	
	static req_t         kos_current_request;
	static CURL*         kos_curl;
	static unsigned char kos_requests_init = 0;
	
	typedef struct {
		unsigned long long code;
		
		#define  MAX_URL_LENGTH_BYTES 8192
		char url[MAX_URL_LENGTH_BYTES];
		
		unsigned long long text_bytes;
		unsigned long long text;
		
	} kos_request_response_t;
	
	void request_global_free(void) {
		requests_close(&kos_current_request);
		
	}
	
	void kos_requests_get(kos_request_response_t* this, const char* url) {
		kos_curl = requests_init(&kos_current_request);
		
		requests_get(kos_curl, &kos_current_request, (char*) url);
		this->code = kos_current_request.code;
		
		unsigned long long url_bytes = strlen(kos_current_request.url);
		if (url_bytes >= MAX_URL_LENGTH_BYTES) {
			printf("WARNING Request URL is too long (%lld bytes, %s)\n", url_bytes, kos_current_request.url);
			strcpy(this->url, "__URL_TOO_LONG__");
			
		} else {
			strcpy(this->url, kos_current_request.url);
			
		}
		
		this->text_bytes = kos_current_request.size + 1;
		this->text       = (unsigned long long) malloc(this->text_bytes);
		
		memcpy((char*) this->text, kos_current_request.text, this->text_bytes);
		
	}
	
	void kos_requests_free(kos_request_response_t* this) {
		if (this->text) {
			free((void*) this->text /* this->text_bytes */);
			
		}
		
	}
	
	static void requests_device_handle(unsigned long long** result, const char* data) {
		unsigned long long* command = (unsigned long long*) data;
		
		if      (command[0] == 'g') kos_requests_get ((kos_request_response_t*) command[1], (const char*) command[2]);
		else if (command[0] == 'f') kos_requests_free((kos_request_response_t*) command[1]);
		else KOS_DEVICE_COMMAND_WARNING("requests")
		
	}
	
#endif
