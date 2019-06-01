#ifndef __AQUA__SDL2_SRC_KOS_MACHINE_H
	#define __AQUA__SDL2_SRC_KOS_MACHINE_H
	
	typedef struct {
		unsigned char exists;
		char* path;
		
		unsigned long long width;
		unsigned long long height;
		
		unsigned char text_only;
		
		texture_t most_recent_texture;
		int pid;
		
	} machine_t;
	
	static unsigned long long root_mid      = 0;
	static unsigned long long machine_count = 0;
	static machine_t* machines;
	
	static unsigned long long __create_machine(unsigned long long path, unsigned long long width, unsigned long long height, unsigned long long text_only) {
		unsigned long long mid = machine_count++;
		
		if (machine_count) machines = (machine_t*) realloc(machines, sizeof(machine_t) * machine_count);
		else               machines = (machine_t*) malloc           (sizeof(machine_t));
		
		machines[mid].exists = 1;
		machines[mid].path   = (char*) path;
		
		machines[mid].width  = width;
		machines[mid].height = height;
		
		machines[mid].text_only = text_only;
		
		machines[mid].most_recent_texture = 0;
		return mid;
		
	}
	
	static void pop_machine(void) {
		machines[machine_count--].exists = 0;
		
		if (machine_count > 0) machines = (machine_t*) realloc(machines, sizeof(machine_t) * machine_count);
		else                   mfree(machines, machine_count * sizeof(machine_t));
		
	}
	
	#include <sys/wait.h>
	static unsigned long long execute_machine(unsigned long long mid, unsigned long long _wait) {
		char ascii_width    [16];
		char ascii_height   [16];
		char ascii_text_only[2] = {machines[mid].text_only ? 'x' : 'g', 0};
		
		char ascii_pid      [16];
		
		sprintf(ascii_pid,    "%d",   getpid());
		
		sprintf(ascii_width,  "%lld", machines[mid].width);
		sprintf(ascii_height, "%lld", machines[mid].height);
		
		char* argv[] = {first_argv, machines[mid].path, ascii_text_only, ascii_width, ascii_height, ascii_pid, (void*) 0};
		int status = 0;
		
		machines[mid].pid = fork();
		if (!machines[mid].pid) {
			execvp(argv[0], argv);
			
		} if (_wait) {
			wait(&status);
			status = WEXITSTATUS(status);
			
		}
		
		return status;
		
	}
	
	unsigned long long create_machine(unsigned long long __path, unsigned long long width, unsigned long long height, unsigned long long text_only) {
		GET_PATH(__path)
		unsigned long long mid = __create_machine((unsigned long long) path, width, height, text_only);
		return mid;
		
	}
	
	texture_t get_machine_texture(unsigned long long mid) {
		if (machines[mid].most_recent_texture) {
			texture_remove(machines[mid].most_recent_texture);
			
		}
		
		unsigned long long bytes = 4 * machines[mid].width * machines[mid].height;
		void* data = (void*) malloc(bytes);
		
		#if __HAS_X11
			XImage* ximage;
		#endif
		
		machines[mid].most_recent_texture = __texture_create(data, 32, machines[mid].width, machines[mid].height, 0);
		mfree(data, bytes);
		return machines[mid].most_recent_texture;
		
	}
	
	#include <signal.h>
	void kill_machine(unsigned long long mid) {
		if (machines[mid].exists) kill(machines[mid].pid, SIGKILL);
		else printf("WARNING Machine (%lld) does not exist\n", mid);
		
	}
	
	void give_machine_events(unsigned long long mid, events_t* events) {
		printf("TODO %s\n", __func__);
		
	}
	
	unsigned long long current_machine(void) {
		return root_mid;
		
	}
	
	void free_all_machines(void) {
		if (!machine_count) {
			return;
			
		}
		
		for (unsigned long long i = 0; i < machine_count; i++) {
			if (machines[i].most_recent_texture) texture_remove(machines[i].most_recent_texture);
			
		}
		
		mfree(machines, machine_count * sizeof(machine_t));
		
	}
	
#endif
