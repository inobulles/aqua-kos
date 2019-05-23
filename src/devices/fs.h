
#ifndef __AQUA__KOS_DEVICES_FS_H
	#define __AQUA__KOS_DEVICES_FS_H
	
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <dirent.h>
	
	int fs_mkdir(const char* __path) {
		char* path = (char*) __path;
		int error = 0;
		
		int path_length = strlen(path);
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
	
	static void fs_device_handle(unsigned long long** result, const char* data) {
		const unsigned long long* fs_command = (const unsigned long long*) data;
		GET_PATH((char*) fs_command[1]);
		
		if      (fs_command[0] == 'm') kos_bda_implementation.temp_value = (unsigned long long) fs_mkdir(path);
		else if (fs_command[0] == 'r') kos_bda_implementation.temp_value = (unsigned long long) remove_directory_recursive(path);
		
		else if (fs_command[0] == 'c') { // move
			GET_PATH_NAME(destination, (char*) fs_command[2]);
			kos_bda_implementation.temp_value = (unsigned long long) rename(path, destination);
			
		} else {
			KOS_DEVICE_COMMAND_WARNING("fs")
			
		}
		
		*result = (unsigned long long*) &kos_bda_implementation.temp_value;
		
	}
	
#endif
