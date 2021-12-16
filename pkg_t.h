#if !defined(__AQUA_KOS__PKG_T)
#define __AQUA_KOS__PKG_T

#include <iar.h>

typedef enum {
	PKG_START_NONE = 0,
	PKG_START_ZED, PKG_START_NATIVE, PKG_START_SYSTEM,
	PKG_START_LEN
} pkg_start_t;

typedef struct {
	const char* path;
	iar_file_t iar;
	
	uint64_t entry_bytes;
	void* entry_data;

	char* cwd;

	// mandatory nodes

	char* _start;
	pkg_start_t start;
	
	char* entry;

	// optional nodes

	char* unique;
	char* name;
} pkg_t;

#endif