void kos_get_platform    (void) { printf("IMPLEMENT %s\n", __func__); }
void kos_platform_command(void* zvm, const char* command) { system(command); } /// REMME
void kos_get_requests    (void* zvm, const char* command) { system(command); } /// REMME
void kos_native          (void) { printf("IMPLEMENT %s\n", __func__); }

#include <zvm.h>
static zvm_program_t* de_program;

static int start_zed(pkg_t* pkg) {
	int rv = -1;

	INFO("Loading ZED ROM ...\n")

	de_program = calloc(1, sizeof *de_program);
	de_program->rom = pkg->entry_data;

	INFO("Starting run setup phase ...\n")

	if (zvm_program_run_setup_phase(de_program)) {
		ERROR("The ZVM's program setup phase failed\n")
		goto done;
	}

	while (!zvm_program_run_loop_phase(de_program)) {
		// do nothing
	}

	rv = de_program->error_code;

done:

	zvm_program_free(de_program);
	free(de_program);

	return rv;
}