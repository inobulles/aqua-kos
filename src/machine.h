
#ifndef __AQUA__SDL2_SRC_KOS_MACHINE_H
	#define __AQUA__SDL2_SRC_KOS_MACHINE_H
	
	typedef struct {
		unsigned char exists;
		char* path;
		
		unsigned long long width;
		unsigned long long height;
		
		unsigned char text_only;
		unsigned long long heap_size;
		
		void* image_data;
		
	} machine_t;
	
	static unsigned long long root_mid      = 0;
	static unsigned long long machine_count = 0;
	static machine_t* machines;
	
	static unsigned long long __create_machine(unsigned long long path, unsigned long long width, unsigned long long height, unsigned long long text_only, unsigned long long heap_size) {
		unsigned long long mid = machine_count++;
		
		if (machine_count) machines = (machine_t*) realloc(machines, sizeof(machine_t) * machine_count);
		else               machines = (machine_t*) malloc           (sizeof(machine_t));
		
		machines[mid].exists = 1;
		machines[mid].path   = (char*) path;
		
		machines[mid].width  = width;
		machines[mid].height = height;
		
		machines[mid].text_only = text_only;
		machines[mid].heap_size = heap_size;
		
		return mid;
		
	}
	
	static void pop_machine(void) {
		machines[machine_count--].exists = 0;
		
		if (machine_count > 0) machines = (machine_t*) realloc(machines, sizeof(machine_t) * machine_count);
		else                   mfree(machines, machine_count * sizeof(machine_t));
		
	}
	
	static unsigned long long open_machine(unsigned long long mid) {
		char ascii_width    [16];
		char ascii_height   [16];
		char ascii_text_only[2] = {machines[mid].text_only ? 'x' : 'g', 0};
		
		char ascii_pid      [16];
		char ascii_heap     [16];
		
		sprintf(ascii_pid,    "%d",   getpid());
		sprintf(ascii_heap,   "%lld", machines[mid].heap_size);
		
		sprintf(ascii_width,  "%lld", machines[mid].width);
		sprintf(ascii_height, "%lld", machines[mid].height);
		
		char* argv[] = {first_argv, machines[mid].path, ascii_text_only, ascii_width, ascii_height, ascii_heap, ascii_pid, NULL};
		
		if (!fork()) {
			execvp(argv[0], argv);
			
		}
		
		return 0;
		
	}
	
	unsigned long long create_machine(unsigned long long __path, unsigned long long width, unsigned long long height, unsigned long long text_only, unsigned long long heap_size) {
		GET_PATH(__path)
		unsigned long long mid = __create_machine((unsigned long long) path, width, height, text_only, heap_size);
		
		if (open_machine(mid)) {
			printf("WARNING Failed to open machine, popping it ...\n");
			pop_machine();
			return -1;
			
		}
		
		return mid;
		
	}
	
	void free_all_machines(void) {
		if (!machine_count) {
			return;
			
		}
		
		for (unsigned long long i = 0; i < machine_count; i++) {
			// machines[i]
			
		}
		
		mfree(machines, machine_count * sizeof(machine_t));
		
	}
	
#endif
