static int start_system(pkg_t* pkg) {
	LOG_WARN("This is currently quite dangerous and needs to be reworked")

	int rv = -1;

	if (!pkg->unique) {
		LOG_ERROR("Unique node is required for system ZPK's")
		goto done;
	}

	LOG_INFO("Chrooting to data directory ...")

	if (chdir("data"));

	if (chdir(pkg->unique) < 0) {
		goto done;
	}

	LOG_INFO("Extracting package contents ...")

	char* extract_cmd = malloc(strlen(pkg->path) + 256 /* should be enough */);
	sprintf(extract_cmd, "iar --unpack %s --output .", pkg->path);

	if (system(extract_cmd)) {
		goto done;
	}

	if (system("rsync -a .package/ ./"));
	if (system("rm -r .package"));

	LOG_INFO("Loading system script ...")

	rv = system(pkg->entry_data);

done:

	return rv;
}
