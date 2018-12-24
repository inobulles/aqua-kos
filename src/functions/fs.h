
#ifndef __AQUA__SDL2_SRC_FUNCTIONS_FS_H
	#define __AQUA__SDL2_SRC_FUNCTIONS_FS_H
	
	#include "../macros_and_inclusions.h"
	
	unsigned long long fs_support(void) {
		return 1;
		
	}
	
	#define FS_CHECK_FILE(action) \
		if (!file) { \
			printf("WARNING File `%s` could not be opened (for %s)\n", path, action); \
			return 1; \
		}
	
	static unsigned long long __fs_read(unsigned long long _path, unsigned long long data, unsigned long long bytes, unsigned long long offset) {
		GET_PATH((char*) _path);
		
		#if KOS_USES_JNI
			if (load_asset_bytes(path, (char**) data, (unsigned long long*) bytes)) {
				ALOGE("WARNING File `%s` could not be opened (for reading)\n", path);
				return 1;
				
			}
		#else
			FILE* file = fopen(path, "rb");
			FS_CHECK_FILE("reading")
			
			fseek(file, 0, SEEK_END);
			*((unsigned long long*) bytes) = ftell(file) + offset;
			rewind(file);
			
			*((char**) data) = (char*) malloc(*((unsigned long long*) bytes) + 1);
			fread(*((char**) data) + offset,  *((unsigned long long*) bytes) - offset, sizeof(char), file);
			
			fclose(file);
		#endif
		
		return 0;
		
	}
	
	unsigned long long fs_read(unsigned long long _path, unsigned long long data, unsigned long long bytes) {
		return       __fs_read(                   _path,                    data,                    bytes, 0);
		
	}
	
	void fs_free(unsigned long long data, unsigned long long bytes) {
		free((char*) data);
		
	}

	#if KOS_USES_JNI
		#define FS_WRITE_CHECK_DEFAULT_ASSETS false
	#endif

	unsigned long long fs_write(unsigned long long _path, unsigned long long data, unsigned long long bytes) {
		#if KOS_USES_JNI
			extern bool default_assets;
		
			#if FS_WRITE_CHECK_DEFAULT_ASSETS
				if (!default_assets) {
			#endif
		#endif
		
		GET_PATH((char*) _path);
		char* final_path = path;
		
		#if KOS_USES_JNI
			SET_FINAL_PATH
		#endif
		
		FILE* file = fopen(final_path, "wb");
		
		#if KOS_USES_JNI
			free(final_path);
		#endif
		
		FS_CHECK_FILE("writing")
		
		fwrite((const void*) ((const char*) data), sizeof(char), bytes, file);
		fclose(file);
		
		return 0;
		
		#if KOS_USES_JNI
			#if FS_WRITE_CHECK_DEFAULT_ASSETS
				} else {
					ALOGE("WARNING `default_assets` is set\n");
					return 1;
		
				}
			#endif
		#endif
		
	}
	
	#include "fs_list.h"
	
#endif
