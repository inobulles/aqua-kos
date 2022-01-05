#include "pkg_t.h"

static void free_pkg(pkg_t* pkg) {
	iar_close(&pkg->iar);

	if (pkg->path) {
		free(pkg->path);
	}

	if (pkg->entry_data) {
		free(pkg->entry_data);
	}

	if (pkg->cwd) {
		free(pkg->cwd);
	}
	
	if (pkg->_start) {
		free(pkg->_start);
	}

	if (pkg->entry) {
		free(pkg->entry);
	}

	if (pkg->unique) {
		free(pkg->unique);
	}

	if (pkg->name) {
		free(pkg->name);
	}

	free(pkg);
}

static void* pkg_read(pkg_t* pkg, const char* key, iar_node_t* parent, uint64_t* bytes_ref) {
	if (!parent) { // if 'parent == NULL', assume root node
		parent = &pkg->iar.root_node;
	}
	
	char* data = NULL;
	iar_node_t node = { 0 };

	if (iar_find_node(&pkg->iar, &node, key, parent) == -1) {
		goto error;
	}

	if (!node.data_bytes) {
		goto error;
	}

	data = malloc(node.data_bytes + 1);
	data[node.data_bytes] = 0; // to be safe

	if (iar_read_node_content(&pkg->iar, &node, data)) {
		goto error;
	}

	if (bytes_ref) {
		*bytes_ref = node.data_bytes;
	}

done:

	return data;

error:

	if (data) {
		free(data);
		data = NULL;
	}

	goto done;
}

static pkg_t* create_pkg(const char* path) {
	int rv = -1;
	
	pkg_t* pkg = calloc(1, sizeof *pkg);
	pkg->path = strdup(path);

	if (iar_open_read(&pkg->iar, pkg->path)) {
		ERROR("Failed to read package IAR file (%s)\n", pkg->path)
		goto error;
	}

	// look for mandatory nodes

	pkg->_start = pkg_read(pkg, "start", NULL, NULL);

	if (!pkg->_start) {
		ERROR("Failed to find start node in package\n")
		goto error;
	}

	pkg->start = PKG_START_NONE;

	if (strncmp(pkg->_start, "zed", 3) == 0) {
		pkg->start = PKG_START_ZED;
	}

	else if (strncmp(pkg->_start, "native", 6) == 0) {
		pkg->start = PKG_START_NATIVE;
	}

	else if (strncmp(pkg->_start, "system", 6) == 0) {
		pkg->start = PKG_START_SYSTEM;
	}

	else {
		ERROR("Unknown start command '%s'\n", pkg->_start)
		goto error;
	}

	pkg->entry = pkg_read(pkg, "entry", NULL, NULL);

	if (!pkg->entry) {
		ERROR("Failed to find entry node in package\n")
		goto error;
	}

	// look for optional nodes

	pkg->unique = pkg_read(pkg, "unique", NULL, NULL);

	if (!pkg->unique) {
		WARN("Package doesn't contain any unique node; the data drive won't be accessible by the application\n")
	}

	pkg->name = pkg_read(pkg, "name", NULL, NULL);

done:

	return pkg;

error:

	free_pkg(pkg);
	pkg = NULL;

	goto done;
}

static int pkg_read_entry(pkg_t* pkg) {
	pkg->entry_data = pkg_read(pkg, pkg->entry, NULL, &pkg->entry_bytes);

	if (!pkg->entry_data) {
		ERROR("Failed to read entry (%s) in package\n", pkg->entry)
		return -1;
	}

	return 0;
}

static int pkg_create_data_dir(pkg_t* pkg) {
	if (!pkg->unique) {
		return -1;
	}

	if (!root_path) {
		return -1;
	}

	if (chdir(root_path) < 0) {
		WARN("Failed to enter the root directory (%s)\n", root_path)
		return -1;
	}

	int rv = -1;
	pkg->cwd = getcwd(NULL, 0);

	mkdir("data", 0700);

	if (chdir("data") < 0) {
		WARN("Failed to enter the data directory (%s/data)\n", root_path)
		goto error;
	}

	if (mkdir(pkg->unique, 0700) < 0 && errno != EEXIST) {
		WARN("Failed to create private data directory for package (%s/data/%s)\n", root_path, pkg->unique)
		goto error;
	}

	// success

	rv = 0;

error:

	if (pkg->cwd) {
		chdir(pkg->cwd);
		free(pkg->cwd);
	}

	return rv;
}

static int pkg_set_proc_name(pkg_t* pkg) {
	if (!pkg->name) {
		return -1;
	}

#if defined(__FreeBSD__)
	setproctitle("%s", pkg->name);
	if (0)

#elif __linux__
	if (prctl(PR_SET_NAME, name, NULL, NULL, NULL) < 0)
#endif
	{
		WARN("Something went wrong setting the process name to %s\n", pkg->name)
		return -1;
	}

	return 0;
}

#include "start/dummy.h"
#include "start/zed.h"
#include "start/native.h"
#include "start/system.h"

static int (*START_LUT[PKG_START_LEN]) (pkg_t* pkg);

static void pkg_init(void) {
	for (unsigned i = 0; i < sizeof(START_LUT) / sizeof(*START_LUT); i++) {
		START_LUT[i] = start_dummy;
	}

	START_LUT[PKG_START_ZED] = start_zed;
	START_LUT[PKG_START_NATIVE] = start_native;
	START_LUT[PKG_START_SYSTEM] = start_system;
}

static int pkg_exec(pkg_t* pkg) {
	if (*START_LUT != start_dummy) {
		pkg_init();
	}

	return START_LUT[pkg->start](pkg);
}

static int pkg_boot(pkg_t* pkg) {
	char* path = realpath(pkg->path, NULL);

	if (!fork()) {
		// TODO see if there's a better way than running the KOS like this

		execlp(exec_name, exec_name, "--boot", path, NULL);
		exit(1);
	}

	free(path);
	return 0;
}