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

// logging

static unsigned verbose = 0;

#define INFO(...) if (verbose) printf("[AQUA KOS] " __VA_ARGS__);
#define WARN(...) fprintf(stderr, "[AQUA KOS] WARNING " __VA_ARGS__);
#define ERROR(...) fprintf(stderr, "[AQUA KOS] ERROR " __VA_ARGS__);

// important global variables

#include "pkg_t.h"
static pkg_t* boot_pkg;

static char* root_path = NULL;
static char* boot_path = NULL;

static uint32_t kos_argc;
static char** kos_argv;

static char* exec_name = NULL;
static char** proc_argv = NULL;

// includes

#include "devices.h"
#include "pkg.h"

int main(int argc, char** argv) {
	int rv = -1;

	exec_name = strdup(argv[0]); // don't worry about freeing this
	proc_argv = argv; // for 'pkg_set_proc_name' to set the name of our process

	root_path = KOS_DEFAULT_ROOT_PATH;
	boot_path = KOS_DEFAULT_BOOT_PATH;
	device_path = KOS_DEFAULT_DEVICES_PATH;

	kos_argc = 0;
	kos_argv = NULL;

	INFO("Parsing arguments ...\n")

	for (int i = 1; i < argc; i++) {
		if (strncmp(argv[i], "--", 2)) {
			ERROR("Unexpected argument '%s'\n", argv[i])
			goto error;
		}

		char* option = argv[i] + 2;

		if (strcmp(option, "verbose") == 0) {
			verbose = 1;
		}

		else if (strcmp(option, "devices") == 0) {
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

	boot_path = realpath(boot_path, NULL); // no risk of memory leak

	if (!boot_path) {
		ERROR("Boot package at '%s' doesn't exist\n", boot_path)
		goto error;
	}

	INFO("Reading the boot package (%s) ...\n", boot_path)
	boot_pkg = create_pkg(boot_path);

	if (!boot_pkg) {
		goto error;
	}

	// setup package for execution

	pkg_set_proc_name(boot_pkg);
	pkg_create_data_dir(boot_pkg);

	// setup devices

	INFO("Setting up devices ...")

	if (setup_devices() < 0) {
		ERROR("Failed to setup devices")
		goto error;
	}

	// change into root directory if it exists

	if (root_path) {
		INFO("Changing into root directory ...\n")
		chdir(root_path);
	}

	// read entry

	if (pkg_read_entry(boot_pkg) < 0) {
		goto error_dev;
	}

	rv = pkg_exec(boot_pkg);
	INFO("Done\n")

error_dev:

	INFO("Unloading devices ...\n")
	unload_devices();

error:

	if (boot_pkg) {
		free_pkg(boot_pkg);
	}

	return rv;
}
