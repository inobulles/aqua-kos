
// manage environment macros

#ifndef KOS_VIDEO_WIDTH
	#define KOS_VIDEO_WIDTH 800
#endif

#ifndef KOS_VIDEO_HEIGHT
	#define KOS_VIDEO_HEIGHT 480
#endif

#ifndef KOS_VSYNC
	#define KOS_VSYNC 1
#endif

#ifndef KOS_MSAA
	#define KOS_MSAA 1
#endif

// manage platforms

#define KOS_PLATFORM_DESKTOP  0
#define KOS_PLATFORM_BROADCOM 1
#define KOS_PLATFORM_ANDROID  2

#ifndef KOS_PLATFORM
	#error "No KOS platform specified"
#endif

#define UMAX 0xFFFFFFFFFFFFFFFF
#define PRECISION 1000000

#include "common.h"

#if   KOS_PLATFORM == KOS_PLATFORM_DESKTOP
	#include "platforms/desktop.h"
#elif KOS_PLATFORM == KOS_PLATFORM_BROADCOM
	#include "platforms/broadcom.h"
#elif KOS_PLATFORM == KOS_PLATFORM_ANDROID
	#include "platforms/android.h"
#else
	#error "KOS platform specified does not exist"
#endif

#include "functions/video.h"
#include "functions/events.h"
#include "functions/device.h"

// kos functions

void load_kos(void) {
	// load platform specific kos
	
	load_platform_kos();
	
	// setup signals
	
	doomed = 0; // not doomed... yet
	struct sigaction signal_interrupt_handler;
	signal_interrupt_handler.sa_handler = catch_signal;
	sigemptyset(&signal_interrupt_handler.sa_mask);
	sigaction(SIGINT, &signal_interrupt_handler, NULL);
	
	// setup gl
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glEnable(GL_DEPTH_TEST);
	glAlphaFunc(GL_GREATER, 0.0f);
	
	glMatrixMode(GL_PROJECTION);
	glViewport(0, 0, (GLsizei) video_width, (GLsizei) video_height);
	glLoadIdentity();
	
	glOrtho(-1.0f, 1.0f, -1.0f, 1.0f, -100.0f, 500.0f);
	glTranslatef(0.0f, 0.0f, -100.0f);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	// print out info about opengl
	
	printf("OpenGL info\n");
	printf("\tVendor:                   %s\n", glGetString(GL_VENDOR));
	printf("\tRenderer:                 %s\n", glGetString(GL_RENDERER));
	printf("\tVersion:                  %s\n", glGetString(GL_VERSION));
	printf("\tShading language version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	
	// load devices
	
	load_devices();
}

void quit_kos(void) {
	quit_devices(); // quit devices
	quit_platform_kos(); // quit platform specific kos
}
