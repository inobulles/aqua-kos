// TODO this is quite dangerous and needs to be reworked

static int start_system(pkg_t* pkg) {
	int rv = -1;

	if (!pkg->unique) {
		ERROR("Unique node is required for system ZPK's\n")
		goto done;
	}

	INFO("Chrooting to data directory ...\n")

	chdir("data");
	
	if (chdir(pkg->unique) < 0) {
		goto done;
	}

	INFO("Extracting package contents ...\n")

	char* extract_cmd = malloc(strlen(pkg->path) + 256 /* should be enough */);
	sprintf(extract_cmd, "iar --unpack %s --output .", pkg->path);

	if (system(extract_cmd)) {
		goto done;
	}

	system("rsync -a .package/ ./");
	system("rm -r .package");

	INFO("Loading system script ...\n")

	rv = system(pkg->entry_data);

done:

	return rv;
}