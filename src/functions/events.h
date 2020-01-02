
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

void kos_get_events(uint64_t __zvm, uint64_t __events) {
	events_t* events = (events_t*) __events;
	memset(events, 0, sizeof(*events));
	
	if (doomed) events->quit = 1;
	else get_events(events);
}
