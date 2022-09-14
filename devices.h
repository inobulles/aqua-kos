
#include <dlfcn.h>

char* device_path = NULL;
#define KOS_BDA

uint64_t  kos_bda_bytes = 0;
uint64_t* kos_bda = NULL;

typedef struct {
	char* name;
	char* path;
	void* lib;

	// functions

	int (*load) (void);
	void (*quit) (void);

	uint64_t (*send) (uint16_t cmd, void* data);

	// hot reloading stuff

#if defined(KOS_HR)
	uv_thread_t hr_thread;
	uv_rwlock_t hr_rwlock;

	uv_loop_t hr_loop;
	uv_fs_event_t* hr_fs_event;
#endif
} device_t;

static uint32_t device_count;
static device_t** devices;

static int setup_devices(void) {
	devices = malloc(sizeof *devices);
	device_count = 1;

	// we want to create a first 'null' device at index '0'
	// this is so that we can use this index as an erroneous return value for the 'query_device' kfunc

	devices[0] = calloc(1, sizeof *devices[0]);
	devices[0]->name = strdup("null");

	return 0;
}

static void free_device_lib(device_t* device) {
	if (device->quit) {
		device->quit();
	}

	if (device->lib) {
		dlclose(device->lib);
	}
}

static void free_device(device_t* device) {
	free_device_lib(device);

	if (device->path) {
		free(device->path);
	}

	if (device->name) {
		free(device->name);
	}

#if defined(KOS_HR)
	uv_stop(&device->hr_loop);

	uv_thread_join(&device->hr_thread);
	uv_rwlock_destroy(&device->hr_rwlock);

	uv_loop_close(&device->hr_loop);

	if (device->hr_fs_event) {
		free(device->hr_fs_event);
	}
#endif

	free(device);
}

static void unload_devices(void) {
	for (size_t i = 0 /* don't forget to include the 'null' device */; i < device_count; i++) {
		free_device(devices[i]);
	}

	free(devices);
}

void device_hr_thread(void* _device);
uint64_t kos_query_device(uint64_t _, uint64_t _name);
uint64_t kos_send_device(uint64_t _, uint64_t _device, uint64_t _cmd, uint64_t _data);
uint64_t kos_callback(uint64_t callback, int argument_count, ...);
void* kos_load_device_function(uint64_t _device, const char* name);

static int load_device(device_t* device) {
	// we're using 'RTLD_NOW' here instead of 'RTLD_LAZY' as would normally be preferred
	// since we only have a small number of functions that we know we'll eventually use, it's better to resolve all external symbols straight away

	device->lib = dlopen(device->path, RTLD_NOW);

	if (!device->lib) {
		LOG_WARN("Failed to load the '%s' device library (in '%s', %s)", device->name, device->path, dlerror())
		return -1;
	}

	// clear the last error

	dlerror();

	// find useful symbols in the device library

	device->load = dlsym(device->lib, "load");
	device->quit = dlsym(device->lib, "quit");
	device->send = dlsym(device->lib, "send");

	// set useful symbols in the device library
	// the 'REF' macro simplifies this somewhat by checking and setting these for us

	#define REF(sym) { \
		uint64_t* ref = dlsym(device->lib, #sym); \
		if (ref) { \
			*(ref) = (uint64_t) (intptr_t) sym; \
		} \
	}

	REF(kos_query_device) REF(kos_load_device_function) REF(kos_callback)
	REF(create_pkg      ) REF(free_pkg                )
	REF(pkg_read        ) REF(pkg_boot                )

	char* unique      = boot_pkg->unique;
	char* cwd_path    = boot_pkg->cwd;
	char* unique_path = boot_pkg->unique_path;

	REF(unique     ) REF(cwd_path ) REF(unique_path)
	REF(device_path) REF(boot_path)
	REF(root_path  ) REF(conf_path)
	REF(kos_bda    )
	REF(kos_argc   ) REF(kos_argv )

	// attempt to load the device

	if (device->load && device->load() < 0) {
		LOG_WARN("Something went wrong in trying to load the '%s' device", device->name)
		return -1;
	}

	// if hot reloading is enabled, watch over the file in question

#if defined(KOS_HR)
	uv_thread_create(&device->hr_thread, device_hr_thread, device);
	uv_rwlock_init(&device->hr_rwlock);
#endif

	return 0;
}

// hot reloading stuff

#if defined(KOS_HR)
void device_hr_cb(uv_fs_event_t* handle, const char* filename, int events, int status) {
	device_t* device = handle->data;

	// wait until we're sure the file is there (again)
	// this is because there could be a delay between removing the file (this triggering this callback) and repopulating it

	struct stat sb;

	while (stat(device->path, &sb)) {
		sleep(1);
	}

	// reload the device

	LOG_INFO("The %s device has been modified; it will now be reloaded", device->name)

	uv_rwlock_wrlock(&device->hr_rwlock);

	free_device_lib(device);
	load_device(device);

	uv_rwlock_wrunlock(&device->hr_rwlock);
}

void device_hr_thread(void* _device) {
	device_t* device = _device;

	uv_loop_init(&device->hr_loop);

	device->hr_fs_event = malloc(sizeof *device->hr_fs_event);
	device->hr_fs_event->data = device;

	uv_fs_event_init(&device->hr_loop, device->hr_fs_event);
	uv_fs_event_start(device->hr_fs_event, device_hr_cb, device->path, UV_FS_EVENT_WATCH_ENTRY);

	uv_run(&device->hr_loop, UV_RUN_DEFAULT);
}
#endif

// useful functions for devices

void* kos_load_device_function(uint64_t _device, const char* name) {
	device_t* device = devices[_device];
	return dlsym(device->lib, name);
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
		LOG_FATAL("TODO Implement 'zvm_callback'")

		// i'm not too sure how 'zvm_callback' is going to end up being implemented, but it may be necessary for the ZVM instance pointer to be passed to it... in which case, hello refactoring!
		// return zvm_callback(callback, argument_count, arguments);
	}

	else if (boot_pkg->start == PKG_START_NATIVE) {
		uint64_t (*callback_pointer) () = (void*) (intptr_t) callback;

		#define c callback_pointer
		#define a arguments

		// yeah this is ugly - C doesn't provide an alternative unfortunately
		// see 'aquabsd-private/components/experimental-devices/aquabsd.alps.opengl.main/functions.h:76' for more details

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

uint64_t kos_query_device(uint64_t _, uint64_t _name) {
	const char* name = (void*) (intptr_t) _name;

	// check to see if the device has already been loaded
	// return its index if so

	for (size_t i = 0; i < device_count; i++) {
		if (!strncmp(devices[i]->name, name, strlen(devices[i]->name))) {
			return i;
		}
	}

	// load the device if it hasn't yet been loaded

	device_t* device = calloc(1, sizeof *device);
	device->name = strdup(name);

	if (asprintf(&device->path, "%s/%s.device", device_path, device->name))
		;

	if (load_device(device) < 0) {
		free_device(device);
		return 0;
	}

	// add the device and return its index

	devices = realloc(devices, (device_count + 1) * sizeof(*devices));
	devices[device_count] = device;

	return device_count++;
}

uint64_t kos_send_device(uint64_t _, uint64_t _device, uint64_t _cmd, uint64_t _data) {
	device_t* device = devices[_device];
	uint16_t cmd = _cmd;
	void* data = (void*) (intptr_t) _data;

	if (!device->send) {
		LOG_WARN("The '%s' device doesn't seem to have a 'send' function. Is it malformed?", device->name)
		return -1;
	}

#if defined(KOS_HR)
	uv_rwlock_rdlock(&device->hr_rwlock);
#endif

	uint64_t rv = device->send(cmd, data);

#if defined(KOS_HR)
	uv_rwlock_rdunlock(&device->hr_rwlock);
#endif

	return rv;
}
