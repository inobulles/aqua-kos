// compilation macros

#ifndef KOS_DEFAULT_ROOT_PATH
	#define KOS_DEFAULT_ROOT_PATH "root"
#endif

#ifndef KOS_DEFAULT_BOOT_PATH
	#define KOS_DEFAULT_BOOT_PATH "root/boot.zpk"
#endif

#ifndef KOS_DEFAULT_DEVICES_PATH
	#define KOS_DEFAULT_DEVICES_PATH "devices"
#endif

// enabled features

uint8_t feature_devices = 0;
uint8_t feature_video = 0;

// event stuff

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

// video stuff

uint64_t video_width  = 0;
uint64_t video_height = 0;

uint8_t video_msaa = 0;
uint8_t video_vsync = 0;