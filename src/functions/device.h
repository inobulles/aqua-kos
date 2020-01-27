
#define KOS_BDA

uint64_t  kos_bda_bytes = 0;
uint64_t* kos_bda = (uint64_t*) 0;

typedef struct {
	char name[1024];
	void* library;
	
	// functions
	
	void (*load) (void);
	void (*quit) (void);
	
	void (*before_flip) (void);
	void (*after_flip) (void);
	void (*handle) (uint64_t** result_pointer_pointer, void* data);
} device_t;

static uint32_t  device_count = 0;
static device_t* devices[64];

void load_devices(void) {
	printf("Indexing devices ...\n");
	
	#ifndef KOS_DEVICES_PATH
		printf("WARNING Failed to index devices, KOS_DEVICES_PATH is not set\n");
	#else
		devices[0] = (device_t*) malloc(sizeof(device_t));
		memset(devices[0], 0, sizeof(device_t));
		
		strcpy(devices[0]->name, "null");
		device_count++;
		
		DIR* dp = opendir(KOS_DEVICES_PATH);
		if (!dp) {
			printf("WARNING Failed to index devices, could not read directory at KOS_DEVICES_PATH (%s)\n", KOS_DEVICES_PATH);
			return;
		}
		
		char path[1024];
		
		struct dirent* entry;
		while ((entry = readdir(dp)) != NULL) if (*entry->d_name != '.') { // ignore all entries starting with a '.'
			sprintf(path, "%s/%s", KOS_DEVICES_PATH, entry->d_name);
			
			void* device_library = dlopen(path, RTLD_NOW);
			if (!device_library) {
				printf("WARNING Failed to open %s (%s)\n", entry->d_name, dlerror());
				break;
			}
			
			dlerror();
			
			device_t* device = (device_t*) malloc(sizeof(device_t));
			strncpy(device->name, entry->d_name, sizeof(device->name));
			
			// functions
			
			device->load        = dlsym(device_library, "load");
			device->quit        = dlsym(device_library, "quit");
			
			device->before_flip = dlsym(device_library, "before_flip");
			device->after_flip  = dlsym(device_library, "after_flip");
			device->handle      = dlsym(device_library, "handle");
			
			// variables
			
			void** reference_pointer = (void**) 0;
			#define REFERENCE(string, variable) { reference_pointer = dlsym(device_library, (string)); if (reference_pointer) *reference_pointer = &(variable); }
			
			REFERENCE("window_based_mouse_pointer", window_based_mouse)
			if (window_based_mouse) {
				REFERENCE("mouse_x_pointer", mouse_x)
				REFERENCE("mouse_y_pointer", mouse_y)
				
				REFERENCE("mouse_scroll_pointer", mouse_scroll)
				REFERENCE("mouse_button_pointer", mouse_button)
			}
			
			REFERENCE("video_width_pointer", video_width)
			REFERENCE("video_height_pointer", video_height)
			
			REFERENCE("kos_bda_bytes_pointer", kos_bda_bytes)
			REFERENCE("kos_bda_pointer", kos_bda)
			
			if (device->load) {
				device->load();
			}
			
			device->library = device_library;
			devices[device_count++] = device;
		}
		
		closedir(dp);
	#endif
}

void before_flip_devices(void) {
	for (uint32_t i = 0; i < device_count; i++) if (devices[i]->before_flip) {
		devices[i]->before_flip();
	}
}

void after_flip_devices(void) {
	for (uint32_t i = 0; i < device_count; i++) if (devices[i]->after_flip) {
		devices[i]->after_flip();
	}
}

void quit_devices(void) {
	printf("Quitting devices ...\n");
	
	for (uint32_t i = 0; i < device_count; i++) {
		if (devices[i]->quit   ) devices[i]->quit();
		if (devices[i]->library) dlclose(devices[i]->library);
		
		free(devices[i]);
	}
}

uint64_t kos_create_device(uint64_t __zvm, uint64_t __name) {
	const char* name = (const char*) __name;
	
	for (uint32_t i = 0; i < device_count; i++) if (strncmp(devices[i]->name, name, sizeof(devices[i]->name)) == 0) {
		return i;
	}
	
	return 0;
}

void* kos_send_device(uint64_t __zvm, uint64_t device_id, uint64_t __data) {
	void* data = (void*) __data;
	
	memset(kos_bda, 0, kos_bda_bytes);
	uint64_t* result_pointer = &kos_bda[0];
	
	if (device_id < device_count && devices[device_id]->handle) {
		devices[device_id]->handle(&result_pointer, data);
	}
	
	return result_pointer;
}
