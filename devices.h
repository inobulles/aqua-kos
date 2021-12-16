
#include <dlfcn.h>

char* device_path = (char*) 0;
#define KOS_BDA

uint64_t  kos_bda_bytes = 0;
uint64_t* kos_bda = (uint64_t*) 0;

typedef struct {
	char* name;
	void* library;

	// functions

	int (*load) (
		uint64_t (*kos_query_device) (uint64_t _, uint64_t name),
		void* (*kos_load_device_function) (uint64_t __device, const char* name),
		uint64_t (*kos_callback) (uint64_t callback, int argument_count, ...));
	
	void (*quit) (void);

	uint64_t (*send) (uint16_t command, void* data);
} device_t;

static uint32_t device_count;
static device_t** devices;

static int setup_devices(void) {
	devices = malloc(sizeof(*devices));
	device_count = 1;

	// we want to create a first 'null' device at index '0'
	// this is so that we can use this index as an erroneous return value for the 'query_device' kfunc

	devices[0] = malloc(sizeof(*devices[0]));
	memset(devices[0], 0, sizeof(*devices[0]));

	devices[0]->name = malloc(5 /* strlen("null") + 1 */);
	memcpy(devices[0]->name, "null", 5);

	return 0;
}

static void unload_devices(void) {
	for (uint32_t i = 0 /* don't forget to include the 'null' device */; i < device_count; i++) {
		if (devices[i]->quit) {
			devices[i]->quit();
		}

		if (devices[i]->library) {
			dlclose(devices[i]->library);
		}

		free(devices[i]->name);
		free(devices[i]);
	}

	free(devices);
}

// useful functions for devices

void* kos_load_device_function(uint64_t __device, const char* name) {
	device_t* device = devices[__device];
	return dlsym(device->library, name);
}

#define KOS_MAX_CALLBACK_ARGUMENTS 3
typedef uint64_t kos_callback_argument_t;

uint64_t kos_callback(uint64_t callback, int argument_count, ...) {
	if (argument_count > KOS_MAX_CALLBACK_ARGUMENTS) {
		fprintf(stderr, "WARNING Too many arguments are being passed to the 'kos_callback' function (%d, maximum is %d)\n", argument_count, KOS_MAX_CALLBACK_ARGUMENTS);
		return -1;
	}
	
	kos_callback_argument_t arguments[KOS_MAX_CALLBACK_ARGUMENTS] = { 0 };
	
	va_list list;
	va_start(list, argument_count);

	for (int i = 0; i < argument_count; i++) {
		arguments[i] = va_arg(list, kos_callback_argument_t);
	}

	va_end(list); // not too sure what the point of this is lol

	if (boot_pkg->start == PKG_START_ZED) {
		printf("TODO Implement 'zvm_callback'\n");

		// i'm not too sure how 'zvm_callback' is going to end up being implemented, but it may be necessary for the ZVM instance pointer to be passed to it... in which case, hello refactoring!
		// return zvm_callback(callback, argument_count, arguments);
	}

	else if (boot_pkg->start == PKG_START_NATIVE) {
		uint64_t (*callback_pointer) () = (void*) callback;
		
		#define c callback_pointer
		#define a arguments

		// yeah this is ugly, C doesn't provide an alternative unfortunately
		// see 'aqua-unix/src/devices/aquabsd.alps.opengl.main/functions.h' at line 94 for more details

		switch (argument_count) {
			case 0: return c();
			case 1: return c(a[0]);
			case 2: return c(a[0], a[1]);
			case 3: return c(a[0], a[1], a[2]);

			default: return -1; // this will never happen
		}
	}

	return -1;
}

// kfuncs

uint64_t kos_query_device(uint64_t _, uint64_t __name) {
	const char* name = (const char*) __name;
	int name_length = strlen(name);

	// check to see if the device has already been loaded
	// return its index if so

	for (uint32_t i = 0; i < device_count; i++) {
		if (strncmp(devices[i]->name, name, strlen(devices[i]->name)) == 0) {
			return i;
		}
	}

	// load the device if it hasn't yet been loaded

	char* path = malloc(strlen(device_path) + name_length + 9 /* strlen("/") + strlen(".device") + 1 */);
	sprintf(path, "%s/%s.device", device_path, name);

	// we're using 'RTLD_NOW' here instead of 'RTLD_LAZY' as would normally be preferred
	// since we only have a small number of functions that we know we'll eventually use, it's better to resolve all external symbols straight away

	void* device_library = dlopen(path, RTLD_NOW);
	free(path); // we won't be needing this anymore

	if (!device_library) {
		printf("[AQUA KOS] WARNING Failed to load the '%s' device library (in '%s', %s)\n", name, device_path, dlerror());
		return 0;
	}

	// clear the last error
	dlerror();

	device_t* device = malloc(sizeof(*device));
	device->library = device_library;

	device->name = malloc(name_length + 1);
	memcpy(device->name, name, name_length + 1);

	// find useful symbols in the device library

	device->load = dlsym(device->library, "load");
	device->quit = dlsym(device->library, "quit");
	device->send = dlsym(device->library, "send");

	// set useful symbols in the device library
	// the 'REFERENCE' macro simplifies this somewhat by checking and setting these for us

	#define REFERENCE(symbol) { \
		uint64_t* reference = (uint64_t*) dlsym(device->library, #symbol); \
		if (reference) *(reference) = (uint64_t) symbol; \
	}

	REFERENCE(boot_pkg->unique)
	REFERENCE(boot_pkg->cwd)

	REFERENCE(device_path)
	REFERENCE(root_path)
	REFERENCE(boot_path)

	REFERENCE(kos_bda)
	
	REFERENCE(kos_argc)
	REFERENCE(kos_argv)
	
	// attempt to load the device

	if (device->load && device->load(kos_query_device, kos_load_device_function, kos_callback) < 0) {
		printf("[AQUA KOS] WARNING Something went wrong in trying to load the '%s' device\n", name);
		dlclose(device->library);

		free(device->name);
		free(device);
		
		return 0;
	}

	// add the device and return its index

	devices = realloc(devices, (device_count + 1) * sizeof(*devices));
	devices[device_count] = device;
	
	return device_count++;
}

uint64_t kos_send_device(uint64_t _, uint64_t __device, uint64_t __command, uint64_t __data) {
	device_t* device = devices[__device];
	uint64_t command = (uint64_t) __command;
	void* data = (void*) __data;

	if (!device->send) {
		printf("[AQUA KOS] WARNING The '%s' device doesn't seem to have a 'send' function. Is it malformed?\n", device->name);
		return -1;
	}

	return device->send((uint16_t) command, data);
}
