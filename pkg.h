typedef struct {
	const char* path;
	iar_file_t iar;

	// mandatory nodes

	char* start;
	char* entry;

	// optional nodes

	char* unique;
	char* name;
} pkg_t;

static void free_pkg(pkg_t* pkg) {
	if (pkg->unique) {
		free(pkg->unique);
	}
	
	if (pkg->start) {
		free(pkg->start);
	}

	free(pkg);
}

static char* pkg_read(pkg_t* pkg, const char* key, iar_node_t* parent) {
	if (!parent) { // if 'parent == NULL', assume root node
		parent = &pkg->root_node;
	}
	
	char* str = NULL;
	iar_node_t node;

	if (iar_find_node(pkg, &node, key, parent) < 0) {
		goto error;
	}

	if (!node.data_bytes) {
		goto error;
	}

	str = malloc(node.data_bytes);

	if (iar_read_node_content(pkg, &node, str)) {
		goto error;
	}

done:

	return str;

error:

	if (str) {
		free(str);
		str = NULL;
	}

	goto done;
}

static pkg_t* create_pkg(const char* path) {
	int rv = -1;
	
	pkg_t* pkg = calloc(1, sizeof *pkg);
	pkg->path = path;

	if (iar_open_read(&pkg->iar, pkg->path)) {
		goto error;
	}

	// look for mandatory nodes

	pkg->start = pkg_read(pkg, "start", NULL);

	if (!pkg->start) {
		goto error;
	}

	pkg->entry = pkg_read(pkg, "entry", NULL);

	if (!pkg->entry) {
		goto error;
	}

	// look for optional nodes

	pkg->unique = pkg_read(pkg, "unique", NULL);
	pkg->name = process_name(pkg, "name", NULL);

	// success

	rv = 0;

done:

	return rv;

error:

	free_pkg(pkg);
	goto done;
}

static int pkg_create_data_dir(pkg_t* pkg) {
	if (!pkg->unique) {
		return -1;
	}

	if (!root_path) {
		return -1;
	}

	if (chdir(root_path) < 0) {
		return -1;
	}

	int rv = -1;
	char* cwd = getcwd(NULL, 0);

	mkdir("data", 0700);

	if (chdir("data") < 0) {
		goto error;
	}

	if (mkdir(pkg->unique, 0700) < 0 && errno != EEXIST) {
		goto error;
	}

	// success

	rv = 0;

error:

	if (cwd) {
		chdir(cwd);
		free(cwd);
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
		return -1;
	}

	return 0;
}

static int pkg_boot(pkg_t* pkg) {
	char* path = realpath(pkg->path);

	if (!fork()) {
		execve("aqua", (const char*[]) { "--boot", path });
		exit(1);
	}

	return 0;
}