
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/joystick.h>

#define MAX_JOYSTICK_COUNT 16
#define MAX_JOYSTICK_AXIS_COUNT 16

static unsigned long long joystick_count = 0;
static struct { int fd; unsigned long long button_state; int16_t axes[MAX_JOYSTICK_AXIS_COUNT]; } joysticks[MAX_JOYSTICK_COUNT] = { 0 };

static unsigned long long __attribute__((unused)) joystick_axis_count(int fd) {
	uint8_t count;
	return ioctl(fd, JSIOCGAXES, &count) < 0 ? -1 : count;
}

static unsigned long long __attribute__((unused)) joystick_button_count(int fd) {
	uint8_t count;
	return ioctl(fd, JSIOCGBUTTONS, &count) < 0 ? -1 : count;
}

static char joystick_name_buffer[128];

static const char* joystick_name(unsigned long long id) {
	if (!joysticks[id].fd) return "non-existent";
	if (ioctl(joysticks[id].fd, JSIOCGNAME(sizeof joystick_name_buffer), joystick_name_buffer) < 0) strncpy(joystick_name_buffer, "unknown", sizeof joystick_name_buffer);
	
	return joystick_name_buffer;
}

void joystick_init(void) {
	for (int i = 0; i < MAX_JOYSTICK_COUNT; i++) {
		joysticks[i].fd = -1;
	}
}

void joystick_update(void) {
	joystick_count = 0;
	
	for (int i = 0; i < MAX_JOYSTICK_COUNT; i++) {
		char path[128];
		sprintf(path, "/dev/input/js%d", i);
		
		int fd = joysticks[i].fd;
		if (fd < 0) {
			if ((fd = joysticks[i].fd = open(path, O_RDONLY | O_NONBLOCK)) < 0) {
				break;
			}
		}
		
		joystick_count++;
		
		while (1) {
			struct js_event event_buffer[0xFF];
			int bytes = read(fd, event_buffer, sizeof event_buffer);
			
			if (bytes < 0) {
				if (errno != EAGAIN) { // joystick unplugged
					printf("Joystick %d unplugged\n", i);
					memset(&joysticks[i], 0, sizeof joysticks[i]);
					joysticks[i].fd = -1;
				}
				
				break;
			}
			
			unsigned long long button_state = joysticks[i].button_state; // no idea why I have to do this
			int16_t axes[MAX_JOYSTICK_AXIS_COUNT];
			memcpy(axes, joysticks[i].axes, sizeof axes);
			
			for (int i = 0; i < bytes / sizeof *event_buffer; i++) {
				struct js_event* event = &event_buffer[i];
				event->type &= ~JS_EVENT_INIT;
				
				if (event->type == JS_EVENT_BUTTON) {
					if (event->value) button_state |=  (1 << event->number);
					else              button_state &= ~(1 << event->number);
					
				} else if (event->type == JS_EVENT_AXIS) {
					axes[event->number] = event->value;
				}
			}
			
			joysticks[i].button_state = button_state;
			memcpy(joysticks[i].axes, axes, sizeof axes);
		}
	}
}

static unsigned long long joystick_button(unsigned long long id, unsigned long long button) {
	return !!(joysticks[id].button_state & (1 << button));
}

static float joystick_axis(unsigned long long id, unsigned long long axis) {
	return (float) joysticks[id].axes[axis] / 32768.0f;
}

static void joystick_device_handle(unsigned long long** result, const char* data) {
	unsigned long long* command = (unsigned long long*) data;
	unsigned char result_string = 0;
	
	if (command[0] == 'n') {
		result_string = 1;
		strncpy(kos_bda_implementation.temp_string, joystick_name(command[1]), sizeof kos_bda_implementation.temp_string);
		
	}
	
	else if (command[0] == 'c') kos_bda_implementation.temp_value = joystick_count;
	else if (command[0] == 'b') kos_bda_implementation.temp_value = joystick_button(command[1], command[2]);
	else if (command[0] == 'a') kos_bda_implementation.temp_value = (signed long long) (joystick_axis(command[1], command[2]) * FLOAT_ONE);
	//~ else if (command[0] == 'h') kos_bda_implementation.temp_value = kos_joystick_rumble(command[1], command[2], command[3], command[4], command[5]);
	
	*result = result_string ? (unsigned long long*) kos_bda_implementation.temp_string : &kos_bda_implementation.temp_value;
}
