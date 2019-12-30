
// include all platform specific external stuff

#include "bcm_host.h"

#include "GLES2/gl2.h"

#include "GLES/gl.h"
#include "GLES/glext.h"

#include "EGL/egl.h"
#include "EGL/eglext.h"

// include all platform specific kos stuff

static EGLDisplay egl_display;
static EGLContext egl_context;
static EGLSurface egl_surface;

static EGL_DISPMANX_WINDOW_T egl_native_window;

static VCHI_INSTANCE_T vchi_instance;
static VCHI_CONNECTION_T* vchi_connections;

#include "broadcom/video.h"
#include "broadcom/events.h"

// platform specific kos functions

void load_platform_kos(void) {
	// attributes
	
	static const EGLint attribute_list[] = {
		EGL_RED_SIZE,    8,
		EGL_GREEN_SIZE,  8,
		EGL_BLUE_SIZE,   8,
		EGL_ALPHA_SIZE,  8,
		EGL_DEPTH_SIZE, 16,
		EGL_SAMPLES, KOS_MSAA,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_NONE,
	};
	
	static const EGLint context_attributes[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE,
	};
	
	// initialize bcm host
	
	bcm_host_init();
	
	// setup everything to do with videocore communication
	
	vcos_init();
	assert(vchi_initialise(&vchi_instance) == VCHIQ_SUCCESS);
	assert(!vchi_connect(NULL, 0, vchi_instance));
	assert(!vc_vchi_tv_init(vchi_instance, &vchi_connections, 1));
	
	// initialize opengl
	
	egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	assert(egl_display != EGL_NO_DISPLAY);
	assert(eglInitialize(egl_display, NULL, NULL) != EGL_FALSE);
	
	EGLConfig config;
	EGLint num_config;
	
	assert(eglSaneChooseConfigBRCM(egl_display, attribute_list, &config, 1, &num_config) != EGL_FALSE);
	assert(eglBindAPI(EGL_OPENGL_ES_API) != EGL_FALSE);
	
	egl_context = eglCreateContext(egl_display, config, EGL_NO_CONTEXT, context_attributes);
	assert(egl_context != EGL_NO_CONTEXT);
	
	eglSwapInterval(egl_display, KOS_VSYNC);
	
	// initialize display
	
	const int VC_LCD = 0;
	assert(graphics_get_display_size(VC_LCD, (uint32_t*) &egl_native_window.width, (uint32_t*) &egl_native_window.height) >= 0);
	
	VC_RECT_T dst_rect = { .x = 0, .y = 0, .width = egl_native_window.width, .height = egl_native_window.height };
	VC_RECT_T src_rect = { .x = 0, .y = 0, .width = egl_native_window.width << 16, .height = egl_native_window.height << 16 };
	
	DISPMANX_DISPLAY_HANDLE_T dispman_display = vc_dispmanx_display_open(VC_LCD);
	DISPMANX_UPDATE_HANDLE_T dispman_update = vc_dispmanx_update_start(0);
	
	egl_native_window.element = vc_dispmanx_element_add(dispman_update, dispman_display, 0 /* layer */, &dst_rect, 0 /* src */, &src_rect, DISPMANX_PROTECTION_NONE, 0 /* alpha */, 0 /* clamp */, 0 /* transform */);
	vc_dispmanx_update_submit_sync(dispman_update);
	
	egl_surface = eglCreateWindowSurface(egl_display, config, &egl_native_window, NULL);
	assert(egl_surface != EGL_NO_SURFACE);
	assert(eglMakeCurrent(egl_display, egl_surface, egl_surface, egl_context) != EGL_FALSE);
	
	// set common variables
	
	window_based_mouse = 0;
	
	video_width  = egl_native_window.width;
	video_height = egl_native_window.height;
}

void quit_platform_kos(void) {
	
}
