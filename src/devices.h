
#include <dlfcn.h>

char* device_path = NULL;
#define KOS_BDA

uint64_t  kos_bda_bytes = 0;
uint64_t* kos_bda = NULL;

typedef struct {
	char* name;
	void* library;

	// functions

	int (*load) (void);
	void (*quit) (void);

	uint64_t (*send) (uint16_t cmd, void* data);
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

void* kos_load_device_function(uint64_t _device, const char* name) {
	device_t* device = devices[_device];
	return dlsym(device->library, name);
}

#define KOS_MAX_CALLBACK_ARGUMENTS 3
typedef uint64_t kos_callback_argument_t;

uint64_t kos_callback(uint64_t callback, int argument_count, ...) {
	if (argument_count > KOS_MAX_CALLBACK_ARGUMENTS) {
		LOG_WARN("Too many arguments are being passed to the 'kos_callback' function (%d, maximum is %d)", argument_count, KOS_MAX_CALLBACK_ARGUMENTS)
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
		uint64_t (*callback_pointer_0) (void) = (void*) (intptr_t) callback;
		uint64_t (*callback_pointer_1) (uint64_t) = (void*) (intptr_t) callback;
		uint64_t (*callback_pointer_2) (uint64_t, uint64_t) = (void*) (intptr_t) callback;
		uint64_t (*callback_pointer_3) (uint64_t, uint64_t, uint64_t) = (void*) (intptr_t) callback;

		#define a arguments

		// yeah this is ugly - C doesn't provide an alternative unfortunately
		// see 'aquabsd-private/components/experimental-devices/aquabsd.alps.opengl.main/functions.h:76' for more details

		switch (argument_count) {
			case 0: return callback_pointer_0();
			case 1: return callback_pointer_1(a[0]);
			case 2: return callback_pointer_2(a[0], a[1]);
			case 3: return callback_pointer_3(a[0], a[1], a[2]);

			default: return -1; // this will never happen
		}
	}

	return -1;
}

// kfuncs

uint64_t kos_query_device(__attribute__((unused)) uint64_t _, uint64_t __name) {
	const char* name = (const char*) (intptr_t) __name;
	int name_length = strlen(name);

	// check to see if the device has already been loaded
	// return its index if so

	for (uint32_t i = 0; i < device_count; i++) {
		if (strncmp(devices[i]->name, name, strlen(devices[i]->name)) == 0) {
			return i;
		}
	}

	// load the device if it hasn't yet been loaded

	char* path = malloc(strlen(device_path) + name_length + 7 /* strlen("/") + strlen(".vdev") + 1 */);
	sprintf(path, "%s/%s.vdev", device_path, name);

	// we're using 'RTLD_NOW' here instead of 'RTLD_LAZY' as would normally be preferred
	// since we only have a small number of functions that we know we'll eventually use, it's better to resolve all external symbols straight away

	void* device_library = dlopen(path, RTLD_NOW);
	free(path); // we won't be needing this anymore

	if (!device_library) {
		LOG_WARN("Failed to load the '%s' device library (in '%s', %s)", name, device_path, dlerror())
		return 0;
	}

	// clear the last error

	dlerror();

	device_t* device = malloc(sizeof *device);
	device->library = device_library;

	device->name = malloc(name_length + 1);
	memcpy(device->name, name, name_length + 1);

	// find useful symbols in the device library

	device->load = dlsym(device->library, "load");
	device->quit = dlsym(device->library, "quit");
	device->send = dlsym(device->library, "send");

	// set useful symbols in the device library
	// the 'REF' macro simplifies this somewhat by checking and setting these for us

	#define REF(sym) { \
		uint64_t* ref = dlsym(device->library, #sym); \
		if (ref) *(ref) = (uint64_t) (intptr_t) sym; \
	}

	REF(kos_query_device)
	REF(kos_load_device_function)
	REF(kos_callback)

	REF(create_pkg)
	REF(free_pkg)

	REF(pkg_read)
	REF(pkg_boot)
	REF(boot_pkg)

	char* unique = boot_pkg->unique;
	char* cwd_path = boot_pkg->cwd;
	char* unique_path = boot_pkg->unique_path;

	REF(unique)
	REF(cwd_path)
	REF(unique_path)

	REF(device_path)
	REF(boot_path)

	REF(root_path)
	REF(conf_path)

	REF(kos_bda)

	REF(kos_argc)
	REF(kos_argv)

	// attempt to load the device

	if (device->load && device->load() < 0) {
		LOG_WARN("Something went wrong in trying to load the '%s' device", name)
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

uint64_t kos_send_device(__attribute__((unused)) uint64_t _, uint64_t _device, uint64_t _cmd, uint64_t _data) {
	device_t* device = devices[_device];
	uint16_t cmd = (uint16_t) _cmd;
	void* data = (void*) (intptr_t) _data;

	if (!device->send) {
		LOG_WARN("The '%s' device doesn't seem to have a 'send' function. Is it malformed?", device->name)
		return -1;
	}

	return device->send(cmd, data);
}
