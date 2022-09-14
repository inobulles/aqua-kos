// feature test macros

#define __STDC_WANT_LIB_EXT2__ 1 // ISO/IEC TR 24731-2:2010 standard library extensions

#if __linux__
	#define _GNU_SOURCE
#endif

// logging

#include <umber.h>
#define UMBER_COMPONENT "KOS"

// POSIX includes

#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// system-specific includes

#include <sys/mount.h>
#include <sys/stat.h>

#if defined(__FreeBSD__)
	// TODO perhaps use some of the 'procctl' commands to do stuff like forcing ASLR?
	#include <sys/procctl.h>
#elif __linux__
	#include <sys/prctl.h>
#endif

#if __linux__
	#include <sys/mman.h> // for 'memfd_create'
	int memfd_create(const char* name, unsigned int flags) __attribute__((weak)); // for systems with glibc >= 2.27 which should thus have 'memfd_create', but inexplicably don't
#endif

// external includes

#if defined(KOS_UV)
	#include <uv.h>
#endif

// compile time macros

#if !defined(KOS_DEFAULT_ROOT_PATH)
	#define KOS_DEFAULT_ROOT_PATH "root"
#endif

#if !defined(KOS_DEFAULT_BOOT_PATH)
	#define KOS_DEFAULT_BOOT_PATH "root/boot.zpk"
#endif

#if !defined(KOS_DEFAULT_DEVICES_PATH)
	#define KOS_DEFAULT_DEVICES_PATH "devices"
#endif

// important global variables

#include "pkg_t.h"
static pkg_t* boot_pkg = NULL;

static char* boot_path = NULL;

static char* root_path = NULL;
static char* conf_path = NULL;

static uint32_t kos_argc = 0;
static char**   kos_argv = NULL;

static char* exec_name  = NULL;
static char** proc_argv = NULL;

// local includes

#include "devices.h"
#include "pkg.h"

int main(int argc, char** argv) {
	int rv = EXIT_FAILURE;

	LOG_INFO("Parsing arguments ...")

	exec_name = strdup(argv[0]); // don't worry about freeing this
	proc_argv = argv; // for 'pkg_set_proc_name' to set the name of our process

	root_path = KOS_DEFAULT_ROOT_PATH;
	boot_path = KOS_DEFAULT_BOOT_PATH;
	device_path = KOS_DEFAULT_DEVICES_PATH;

	kos_argc = 0;
	kos_argv = NULL;

	for (int i = 1; i < argc; i++) {
		if (strncmp(argv[i], "--", 2)) {
			LOG_FATAL("Unexpected argument '%s'", argv[i])
			goto error;
		}

		char* option = argv[i] + 2;

		if (!strcmp(option, "devices")) {
			device_path = argv[++i];
		}

		else if (!strcmp(option, "root")) {
			root_path = argv[++i];
		}

		else if (!strcmp(option, "boot")) {
			boot_path = argv[++i];
			i++; // skip the argument we're currently parsing (boot_path)

			// all arguments after boot are program arguments

			kos_argc = argc - i;
			kos_argv = argv + i;

			break;
		}
	}

	if (!strcmp(root_path, "NO_ROOT")) {
		root_path = NULL;
	}

	if (root_path) {
		if (asprintf(&conf_path, "%s/conf", root_path)) // no risk of memory leak
			;
	}

	char* _boot_path = boot_path;
	boot_path = realpath(boot_path, NULL); // no risk of memory leak

	if (!boot_path) {
		LOG_FATAL("Boot package at '%s' doesn't exist", _boot_path)
		goto error;
	}

	LOG_INFO("Reading the boot package (%s) ...", boot_path)
	boot_pkg = create_pkg(boot_path);

	if (!boot_pkg) {
		goto error;
	}

	// setup package for execution

	pkg_set_proc_name(boot_pkg);
	pkg_create_data_dir(boot_pkg);

	// inform user that hot reloading is enabled

#if defined(KOS_UV)
	LOG_INFO("Hot reloading enabled (using libuv %d.%d)", UV_VERSION_MAJOR, UV_VERSION_MINOR)
#endif

	// setup devices

	LOG_INFO("Setting up devices ...")

	if (setup_devices() < 0) {
		LOG_FATAL("Failed to setup devices")
		goto error;
	}

	// read entry

	if (pkg_read_entry(boot_pkg) < 0) {
		goto error_dev;
	}

	rv = pkg_exec(boot_pkg);
	LOG_SUCCESS("Done")

error_dev:

	LOG_INFO("Unloading devices ...")
	unload_devices();

error:

	LOG_INFO("Freeing boot package ...")

	if (boot_pkg) {
		free_pkg(boot_pkg);
	}

	LOG_SUCCESS("Bonne journée, au revoir !")

	return rv;
}
