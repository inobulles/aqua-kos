
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#define MAX_PATH_LENGTH 4096

#define default_access_root_path_name "root/"
#define default_access_root_path default_access_root_path_name

static char access_root_path_name[MAX_PATH_LENGTH] = default_access_root_path_name;
static char access_root_path     [MAX_PATH_LENGTH] = default_access_root_path;

#define GET_PATH_NAME(name, _path) \
	char name[MAX_PATH_LENGTH]; \
	strncpy(name, access_root_path_name, sizeof(name)); \
	strncat(name, (char*) (_path), MAX_PATH_LENGTH - strlen(name)); \

#define GET_PATH(_path) GET_PATH_NAME(path, _path)

#define FS_CHECK_FILE(action) \
	if (!file) { \
		printf("WARNING File `%s` could not be opened (for %s)\n", path, action); \
		return 1; \
	}

void* zvm_malloc(uint64_t self, uint64_t bytes);
void  zvm_mfree(uint64_t self, uint64_t pointer, uint64_t bytes);

static unsigned long long __fs_read(unsigned long long _path, unsigned long long data, unsigned long long bytes, unsigned long long offset) {
	GET_PATH((char*) _path);
	
	FILE* file = fopen(path, "rb");
	FS_CHECK_FILE("reading")
	
	fseek(file, 0, SEEK_END);
	*((unsigned long long*) bytes) = ftell(file) + offset;
	rewind(file);
	
	*((char**) data) = (char*) zvm_malloc(0, *((unsigned long long*) bytes) + 1);
	fread(*((char**) data) + offset,              *((unsigned long long*) bytes) - offset, sizeof(char), file);
	
	fclose(file);
	return 0;
	
}

unsigned long long fs_read(unsigned long long _path, unsigned long long data, unsigned long long bytes) {
	return       __fs_read(                   _path,                    data,                    bytes, 0);
	
}

void fs_free(unsigned long long data, unsigned long long bytes) {
	zvm_mfree(0, data, bytes);
	
}

unsigned long long fs_write(unsigned long long _path, unsigned long long data, unsigned long long bytes) {
	GET_PATH((char*) _path);
	char* final_path = path;
	
	FILE* file = fopen(final_path, "wb");
	
	FS_CHECK_FILE("writing")
	
	fwrite((const void*) ((const char*) data), sizeof(char), bytes, file);
	fclose(file);
	
	return 0;
}

#define FS_LIST_D_NAME_VALID (strcmp(directory->d_name, "..") != 0 && strcmp(directory->d_name, ".") != 0)

unsigned long long fs_list_count(unsigned long long _path) {
	GET_PATH((char*) _path);
	unsigned long long count = 0;
	
	DIR* dp = opendir(path);
	struct dirent* directory;
	
	if (dp) {
		while ((directory = readdir(dp)) != NULL) {
			count += FS_LIST_D_NAME_VALID;
			
		}
		
		closedir(dp);
		return count;
		
	} else {
		printf("WARNING Directory `%s` could not be opened (for list counting)\n", path);
		return (unsigned long long) -1;
		
	}
	
}

unsigned long long fs_list(unsigned long long _path, unsigned long long count, unsigned long long __result) {
	char** result = (char**) __result;
	GET_PATH((char*) _path);
	
	if (count == -1) {
		printf("WARNING Failed to open current directory");
		return 1;
		
	}
	
	unsigned long long current = 0;
	
	DIR* dp = opendir(path);
	struct dirent* directory;
	
	if (dp) {
		while ((directory = readdir(dp)) != NULL) {
			if (FS_LIST_D_NAME_VALID) {
				unsigned long long bytes = strlen(directory->d_name) + 1;
				result[current] = (char*) zvm_malloc(0, bytes * sizeof(char));
				memset(result[current], 0, bytes);
				memcpy(result[current++], directory->d_name, bytes);
				
			}
			
		}
		
		closedir(dp);
		
	} else {
		printf("WARNING Directory `%s` could not be opened (for listing)\n", path);
		return 1;
		
	}
	
	return 0;
	
}

#define FS_TYPE_UNKNOWN   0
#define FS_TYPE_DIRECTORY 1
#define FS_TYPE_FILE      2

#include <sys/stat.h>
unsigned long long fs_type(unsigned long long __path) {
	return FS_TYPE_FILE;
	GET_PATH((char*) __path)
	
	DIR* dp = opendir(path);
	struct dirent* directory;
	
	unsigned long long result = (unsigned long long) -1;
	if (dp) {
		if ((directory = readdir(dp)) != NULL) {
			result = FS_TYPE_UNKNOWN;
			
			if (directory->d_type == DT_UNKNOWN) {
				struct stat path_stat;
				stat(directory->d_name, &path_stat);
				
				if      (S_ISDIR(path_stat.st_mode)) result = FS_TYPE_DIRECTORY;
				else if (S_ISREG(path_stat.st_mode)) result = FS_TYPE_FILE;
				
			}
			
			else if (directory->d_type == DT_DIR) result = FS_TYPE_DIRECTORY;
			else if (directory->d_type == DT_REG) result = FS_TYPE_FILE;
			
		} else {
			printf("WARNING fs_type encountered an unknown problem\n");
			
		}
		
	} else {
		printf("WARNING Directory `%s` could not be opened (for type querying)\n", path);
		
	}
	
	return result;
	
}

int fs_mkdir(const char* __path) {
	char* path = (char*) __path;
	int error = 0;
	
	size_t path_length = strlen(path);
	for (int i = 0; i < path_length; i++) {
		if (path[i] == '/') {
			path[i] = 0;
			error += mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
			path[i] = '/';
			
		}
		
	}
	
	error += mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	return error;
	
}

int remove_directory_recursive(const char* name) {
	DIR* directory = opendir(name);
	
	if (directory == NULL) {
		return remove(name);
		
	}
	
	int errors = 0;
	
	struct dirent* entry;
	char path[PATH_MAX];
	
	while ((entry = readdir(directory)) != NULL) {
		if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
			snprintf(path, (size_t) PATH_MAX, "%s/%s", name, entry->d_name);
			
			if (entry->d_type == DT_DIR) errors += remove_directory_recursive(path);
			else                         errors += remove                    (path);
			
		}
		
	}
	
	closedir(directory);
	remove(name);
	
	return errors;
	
}

unsigned long long fs_access(unsigned long long root) {
	#if SYSTEM_ACCESS
		strncpy(access_root_path,      root ? (const char*) root : default_access_root_path,      sizeof(access_root_path));
		strncpy(access_root_path_name, root ? (const char*) root : default_access_root_path_name, sizeof(access_root_path_name));
		
		return 0;
	#endif
	
	return 1;
	
}

static void fs_device_handle(unsigned long long** result, const char* data) {
	unsigned long long* command = (unsigned long long*) data;
	
	if      (command[0] == 'r') { GET_PATH((char*) command[1]); kos_bda_implementation.temp_value = (unsigned long long) fs_read (command[1], command[2], command[3]); } // read
	else if (command[0] == 'w') { GET_PATH((char*) command[1]); kos_bda_implementation.temp_value = (unsigned long long) fs_write(command[1], command[2], command[3]); } // write
	
	else if (command[0] == 'm') { GET_PATH((char*) command[1]); kos_bda_implementation.temp_value = (unsigned long long) fs_mkdir(path); } // make directory
	else if (command[0] == 'd') { GET_PATH((char*) command[1]); kos_bda_implementation.temp_value = (unsigned long long) remove_directory_recursive(path); } // delete
	
	else if (command[0] == 't') { GET_PATH((char*) command[1]); kos_bda_implementation.temp_value = (unsigned long long) fs_type(command[1]); } // type
	
	else if (command[0] == 'c') { GET_PATH((char*) command[1]); kos_bda_implementation.temp_value = (unsigned long long) fs_list_count(command[1]); } // count list
	else if (command[0] == 'l') { GET_PATH((char*) command[1]); kos_bda_implementation.temp_value = (unsigned long long) fs_list(command[1], command[2], command[3]); } // list
	
	else if (command[0] == 'a') kos_bda_implementation.temp_value = (unsigned long long) fs_access(command[1]); // access
	
	else if (command[0] == 'v') { // move
		GET_PATH((char*) command[1]);
		GET_PATH_NAME(destination, (char*) command[2]);
		kos_bda_implementation.temp_value = (unsigned long long) rename(path, destination);
	}
	
	*result = (unsigned long long*) &kos_bda_implementation.temp_value;
}
