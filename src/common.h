typedef struct {
	char message[64];
	
	uint64_t quit;
	uint64_t resize;
	uint64_t macro;
} events_t;

int64_t mouse_x = 0;
int64_t mouse_y = 0;

int64_t mouse_scroll = 0;
uint8_t mouse_button = 0;

uint8_t window_based_mouse = 0;

uint64_t video_width  = 0;
uint64_t video_height = 0;
