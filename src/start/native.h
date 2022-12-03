#include <sys/mman.h>

static int start_native(pkg_t* pkg) {
	int rv = -1;

	// TODO some platforms are not supported. Support them: https://github.com/google/iree/issues/3845
	// also thanks to https://stackoverflow.com/questions/5053664/dlopen-from-memory for introducing me to 'fdlopen' ❤️

	LOG_INFO("Loading native binary ...");

	void* lib = NULL;

	#if defined(__FreeBSD__) // are we running on a supported platform? (i.e. FreeBSD or aquaBSD)
		LOG_INFO("Creating a shared memory file descriptor for the native binary ...")

		int fd = shm_open(SHM_ANON, O_RDWR, 0);

		if (ftruncate(fd, pkg->entry_bytes))
			;

		void* native_bin = mmap(NULL, pkg->entry_bytes, PROT_WRITE, MAP_SHARED, fd, 0);
		memcpy(native_bin, pkg->entry_data, pkg->entry_bytes);

		munmap(native_bin, pkg->entry_bytes);

		LOG_INFO("Dynamically linking native binary ...")

		lib = fdlopen(fd, RTLD_LAZY);
		close(fd);

	#elif __linux__ // are we instead running on Linux? (https://github.com/google/iree/issues/3845)
		LOG_INFO("Creating memory file descriptor for the native binary ...")

		if (!pkg->unique) {
			LOG_ERROR("Unique node is required for native binaries on GNU+Linux")
			goto done;
		}

		char* name = malloc(strlen(pkg->unique) + 17 /* strlen("aqua_native_bin_") + 1 */);
		sprintf(name, "aqua_native_bin_%s", pkg->unique);

		#if defined(__WSL__)
			LOG_INFO("Applying special, special fix for WSL 💛 ...")

			char tmp_file_path[] = "/tmp/aqua-wsl-XXXXXXX";
			int fd = mkstemp(tmp_file_path);

			unlink(tmp_file_path);

		#elif __GLIBC__ < 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ < 27)
			LOG_INFO("Applying fix for glibc < 2.27 (Linux kernel >= 3.17 required for 'memfd_create') ...")

			char tmp_file_path[] = "/tmp/aqua-old-glibc-XXXXXXX";
			int fd = mkstemp(tmp_file_path);

			unlink(tmp_file_path);

		#else
			int fd = memfd_create(name, 0);
		#endif

		if (ftruncate(fd, pkg->entry_bytes));

		void* native_bin = mmap(NULL, pkg->entry_bytes, PROT_WRITE, MAP_SHARED, fd, 0);
		memcpy(native_bin, pkg->entry_data, pkg->entry_bytes);

		munmap(native_bin, pkg->entry_bytes);

		char fd_name[64]; // likely enough space
		snprintf(fd_name, sizeof(fd_name), "/proc/self/fd/%d", fd);

		lib = dlopen(fd_name, RTLD_LAZY);
		close(fd);

	#else // unsupported platform
		LOG_WARN("Running native binary ZPK files is unsupported on this platform (only FreeBSD/aquaBSD and GNU+Linux (and apparently WSL now too) are supported currently)")
	#endif

	if (!lib) {
		LOG_ERROR("Failed to link the native binary (%s)", dlerror())
		goto done;
	}

	// actually enter the native binary

	LOG_INFO("Looking for entry symbol to native binary ...")

	int (*native_bin_entry) (/* &c */) = dlsym(lib, "__native_entry");

	if (!native_bin_entry) {
		LOG_ERROR("Entry symbol not found")
		goto done;
	}

	LOG_INFO("Looking for 'aqua_set_kos_functions' in the native binary and calling it ...")

	void (*aqua_set_kos_functions) (
		uint64_t (*_kos_query_device) (uint64_t _, uint64_t name),
		uint64_t (*_kos_send_device) (uint64_t _, uint64_t device, uint64_t command, uint64_t data)
	) = dlsym(lib, "aqua_set_kos_functions");

	if (aqua_set_kos_functions) {
		aqua_set_kos_functions(kos_query_device, kos_send_device);
	}

	LOG_INFO("Entering into native binary ...")
	rv = native_bin_entry(kos_query_device, kos_send_device);

done:

	return rv;
}
