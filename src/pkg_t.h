#if !defined(__AQUA_KOS__PKG_T)
#define __AQUA_KOS__PKG_T

#include <iar.h>

typedef enum {
	PKG_START_NONE = 0,
	PKG_START_ZED, PKG_START_NATIVE, PKG_START_SYSTEM,
	PKG_START_LEN
} pkg_start_t;

typedef struct {
	char* path;
	iar_file_t iar;

	uint64_t entry_bytes;
	void* entry_data;

	char* cwd;
	char* unique_path;

	// mandatory nodes

	char* _start;
	pkg_start_t start;

	char* entry;

	// optional nodes

	char* unique;
	char* name;
} pkg_t;

static pkg_t* create_pkg(const char* path);
static void free_pkg(pkg_t* pkg);

static void* pkg_read(pkg_t* pkg, const char* key, iar_node_t* parent, uint64_t* bytes_ref);
static int pkg_boot(pkg_t* pkg);

#endif