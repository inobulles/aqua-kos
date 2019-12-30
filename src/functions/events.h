
static uint8_t doomed;
extern void quit_kos(void);

void catch_signal(int signal) {
	doomed = 1;
	
	if (signal == SIGKILL) {
		printf("Recieved SIGKILL, frantically trying to die.\n");
		quit_kos();
		exit(1);
	}
}

void kos_get_events(void* zvm, events_t* events) {
	memset(events, 0, sizeof(*events));
	
	if (doomed) events->quit = 1;
	else get_events(events);
}
