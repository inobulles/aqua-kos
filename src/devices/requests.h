
#ifndef __AQUA__KOS_DEVICES_REQUESTS_H
	#define __AQUA__KOS_DEVICES_REQUESTS_H
	
	typedef struct {
		char* address;
		
	} request_response_t;
	
	#if !KOS_USES_JNI
		int asprintf(char **strp, const char *fmt, ...);
		#include "../external/librequests/src/requests.c"
		
		static req_t         kos_current_request;
		static CURL*         kos_curl;
		static unsigned char kos_requests_init = 0;
	#endif
	
	typedef struct {
		unsigned long long code;
		
		#define  MAX_URL_LENGTH_BYTES 8192
		char url[MAX_URL_LENGTH_BYTES];
		
		unsigned long long text_bytes;
		unsigned long long text;
		
	} kos_request_response_t;
	
	void request_global_free(void) {
		#if !KOS_USES_JNI
			requests_close(&kos_current_request);
		#endif
		
	}
	
	void kos_requests_get(kos_request_response_t* self, const char* url) {
		memset(self, 0, sizeof(*self));
		
		#if !KOS_USES_JNI
			kos_curl = requests_init(&kos_current_request);
			requests_get(kos_curl, &kos_current_request, (char*) url);
			self->code = (unsigned long long) kos_current_request.code;
			unsigned long long url_bytes = strlen(kos_current_request.url);
		#else
			self->code = (unsigned long long) CALLBACK_INT(java_requests_get, callback_env->NewStringUTF(url));
			unsigned long long url_bytes = strlen(url);
		#endif
		
		if (url_bytes >= MAX_URL_LENGTH_BYTES) {
			printf("WARNING Request URL is too long (%lld bytes, %s)\n", url_bytes, url);
			strcpy(self->url, "__URL_TOO_LONG__");
			
		} else {
			#if !KOS_USES_JNI
				strcpy(self->url, kos_current_request.url);
			#else
				strcpy(self->url, url);
			#endif
			
		}
		
		#if !KOS_USES_JNI
			self->text_bytes = kos_current_request.size + 1;
		#else
			self->text_bytes = (unsigned long long) CALLBACK(java_requests_length, callback_env->CallStaticLongMethod, 0) + 1;
		#endif
		
		self->text = (unsigned long long) heap_malloc(self->text_bytes);
		memset((void*) self->text, 0, self->text_bytes);
		
		#if !KOS_USES_JNI
			memcpy((char*) self->text, kos_current_request.text, self->text_bytes);
		#else
			jbyteArray array = (jbyteArray) CALLBACK(java_requests_text, callback_env->CallStaticObjectMethod, 0);
			if (array) callback_env->GetByteArrayRegion(array, 0, callback_env->GetArrayLength(array), (jbyte*) self->text);
		#endif
		
	}
	
	void kos_requests_free(kos_request_response_t* self) {
		if (self->text) {
			heap_mfree(self->text, self->text_bytes);
			
		}
		
	}
	
	static void requests_device_handle(unsigned long long** result, const char* data) {
		unsigned long long* command = (unsigned long long*) data;
		
		if      (command[0] == 'g') kos_requests_get ((kos_request_response_t*) command[1], (const char*) command[2]);
		else if (command[0] == 'f') kos_requests_free((kos_request_response_t*) command[1]);
		else if (command[0] == 'u') asprintf(&custom_ua, "%s", (const char*) command[1]);
		else KOS_DEVICE_COMMAND_WARNING("requests")
		
	}
	
#endif
