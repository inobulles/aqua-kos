
#ifndef __AQUA__SDL2_SRC_KOS_NATIVE_H
	#define __AQUA__SDL2_SRC_KOS_NATIVE_H
	
	static size_t page_size = 0;
	
	unsigned long long native(unsigned long long platform, unsigned long long __pointer, unsigned long long bytes) {
		unsigned char* pointer = (unsigned char*) __pointer;
		unsigned long long system_platform = get_platform();
		
		if (platform != system_platform) {
			printf("WARNING Provided code is for platform 0x%llx, not 0x%llx (system platform)\n", platform, system_platform);
			return 1;
			
		} else if (platform & PLATFORM_UNIX && (defined(__x86_64__) || defined(__i368__))) { // x86/x64 Unix based platform (source: https://stackoverflow.com/questions/37122186/c-put-x86-instructions-into-array-and-execute-them)
			if (!page_size) {
				page_size = sysconf(_SC_PAGESIZE);
				
				if (page_size <= -1) {
					printf("WARNING Could not retreive page size\n");
					return 1;
					
				} else {
					printf("Page size is 0x%llx bytes\n", page_size);
					
				}
				
				size_t rounded_bytes = ((bytes + page_size) / page_size) * page_size;
				void* executable_area = mmap(0, rounded_bytes, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
				
				if (!executable_area) {
					printf("WARNING Failed to create executable area (mmap)\n");
					return 1;
					
				}
				
				memcpy(executable_area, pointer, bytes);
				memset(executable_area + bytes, 0xCC, rounded_bytes - bytes);
				
				if (mprotect(executable_area, rounded_bytes, PROT_READ | PROT_EXEC)) {
					printf("WARNING Failed to make area executable (and unwritable)\n");
					return 1;
					
				}
				
				((void(*)(void)) executable_area)();
				munmap(executable_area, rounded_bytes);
				
			}
			
		} else {
			printf("WARNING Your platform")
			return 1;
			
		}
		
		return 0;
		
	}
	
#endif
