static int start_native(pkg_t* pkg) {
	int rv = -1;

	// TODO some platforms are not supported. Support them: https://github.com/google/iree/issues/3845
	// also thanks to https://stackoverflow.com/questions/5053664/dlopen-from-memory for introducing me to 'fdlopen' ❤️
	
	INFO("Loading native binary ...\n");

	void* lib = NULL;

	#if defined(__FreeBSD__) // are we running on a supported platform? (i.e. FreeBSD or aquaBSD)
		INFO("Creating a shared memory file descriptor for the native binary ...\n")

		int fd = shm_open(SHM_ANON, O_RDWR, 0);
		ftruncate(fd, pkg->entry_bytes);

		void* native_bin = mmap(NULL, pkg->entry_bytes, PROT_WRITE, MAP_SHARED, fd, 0);
		memcpy(native_bin, pkg->entry_data, pkg->entry_bytes);

		munmap(native_bin, pkg->entry_bytes);

		INFO("Dynamically linking native binary ...\n")

		lib = fdlopen(fd, RTLD_LAZY);
		close(fd);
		
	#elif __linux__ // are we instead running on Linux? (https://github.com/google/iree/issues/3845)
		INFO("Creating memory file descriptor for the native binary ...\n")

		if (!unique) {
			ERROR("Unique node is required for native binaries on GNU+Linux\n")
			goto done;
		}

		char* name = malloc(strlen(unique) + 17 /* strlen("aqua_native_bin_") + 1 */);
		sprintf(name, "aqua_native_bin_%s", unique);

		#if defined(__WSL__)
			INFO("Applying special, special fix for WSL 💛 ...\n")

			char tmp_file_path[] = "/tmp/aqua-XXXXXXX";
			int fd = mkstemp(tmp_file_path);

			unlink(tmp_file_path);

		#else
			int fd = memfd_create(name, 0);
		#endif

		ftruncate(fd, pkg->entry_bytes);
		
		void* native_bin = mmap(NULL, pkg->entry_bytes, PROT_WRITE, MAP_SHARED, fd, 0);
		memcpy(native_bin, pkg->entry_data, pkg->entry_bytes);

		munmap(native_bin, pkg->entry_bytes);
		
		char fd_name[64]; // likely enough space
		snprintf(fd_name, sizeof(fd_name), "/proc/self/fd/%d", fd);
		
		lib = dlopen(fd_name, RTLD_LAZY);
		close(fd);

	#else // unsupported platform
		WARN("Running native binary ZPK files is unsupported on this platform (only FreeBSD/aquaBSD and GNU+Linux (and apparently WSL now too) are supported currently)\n")
	#endif

	if (!lib) {
		ERROR("Failed to link the native binary (%s)\n", dlerror())
		goto done;
	}

	// actually enter the native binary

	INFO("Looking for entry symbol to native binary ...\n")
	
	int (*native_bin_entry) (/* &c */) = dlsym(lib, "__native_entry");

	if (!native_bin_entry) {
		ERROR("Entry symbol not found\n")
		goto done;
	}

	INFO("Looking for 'aqua_set_kos_functions' in the native binary and calling it ...\n")
	
	void (*aqua_set_kos_functions) (
		uint64_t (*_kos_query_device) (uint64_t _, uint64_t name),
		uint64_t (*_kos_send_device) (uint64_t _, uint64_t device, uint64_t command, uint64_t data)
	) = dlsym(lib, "aqua_set_kos_functions");

	if (aqua_set_kos_functions) {
		aqua_set_kos_functions(kos_query_device, kos_send_device);
	}

	INFO("Entering into native binary ...\n")
	rv = native_bin_entry(kos_query_device, kos_send_device);

done:

	return rv;
}