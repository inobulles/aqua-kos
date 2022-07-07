// logging

#include <umber.h>
#define UMBER_COMPONENT "KOS"

// includes

#if __linux__
	#define _GNU_SOURCE
	#include <sys/mman.h> // for 'memfd_create'
#endif

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <signal.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>

#include <dirent.h>
#include <sys/stat.h>
#include <sys/mount.h>

#if defined(__FreeBSD__)
	// TODO perhaps use some of the 'procctl' commands to do stuff like forcing ASLR?
	#include <sys/procctl.h>
#elif __linux__
	#include <sys/prctl.h>
#endif

// compile time macros

#ifndef KOS_DEFAULT_ROOT_PATH
	#define KOS_DEFAULT_ROOT_PATH "root"
#endif

#ifndef KOS_DEFAULT_BOOT_PATH
	#define KOS_DEFAULT_BOOT_PATH "root/boot.zpk"
#endif

#ifndef KOS_DEFAULT_DEVICES_PATH
	#define KOS_DEFAULT_DEVICES_PATH "devices"
#endif

// important global variables

#include "pkg_t.h"
static pkg_t* boot_pkg;

static char* root_path = NULL;
static char* boot_path = NULL;

static uint32_t kos_argc;
static char** kos_argv;

static char* exec_name = NULL;
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

		if (strcmp(option, "devices") == 0) {
			device_path = argv[++i];
		}

		else if (strcmp(option, "root") == 0) {
			root_path = argv[++i];
		}

		else if (strcmp(option, "boot") == 0) {
			boot_path = argv[++i];
			i++; // skip the argument we're currently parsing (boot_path)

			// all arguments after boot are program arguments

			kos_argc = argc - i;
			kos_argv = argv + i;

			break;
		}
	}

	if (strcmp(root_path, "NO_ROOT") == 0) {
		root_path = NULL;
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

	if (boot_pkg) {
		free_pkg(boot_pkg);
	}

	return rv;
}
