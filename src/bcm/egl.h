
#ifndef __AQUA__SDL2_SRC_KOS_BCM_EGL_H
	#define __AQUA__SDL2_SRC_KOS_BCM_EGL_H
	
	#include <assert.h>
	
	void bcm_ogles_setup(kos_t* kos) {
		int32_t success = 0;
		EGLint config_num;
		
		static EGL_DISPMANX_WINDOW_T native_window;
		
		DISPMANX_ELEMENT_HANDLE_T dispman_element;
		DISPMANX_DISPLAY_HANDLE_T dispman_display;
		DISPMANX_UPDATE_HANDLE_T  dispman_update;
		
		VC_RECT_T dst_rect;
		VC_RECT_T src_rect;
		
		static const EGLint attribute_list[] = {
			EGL_RED_SIZE,   8,
			EGL_GREEN_SIZE, 8,
			EGL_BLUE_SIZE,  8,
			EGL_ALPHA_SIZE, 8,
			EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
			EGL_NONE,
		};
		
		EGLConfig config;
		
		kos->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
		assert(kos->display != EGL_NO_DISPLAY);
		
		EGLBoolean result = eglInitialize(kos->display, NULL, NULL);
		assert(result != EGL_FALSE);
		
		result = eglChooseConfig(kos->display, attribute_list, &config, 1, &config_num);
		assert(result != EGL_FALSE);
		
		kos->context = eglCreateContext(kos->display, config, EGL_NO_CONTEXT, NULL);
		assert(kos->context != EGL_NO_CONTEXT);
		
		success = graphics_get_display_size(0, &kos->width, &kos->height);
		printf("BCM OpenGL ES detected display (width = %d, height = %d)\n", kos->width, kos->height);
		assert(success >= 0);
		
		dst_rect.x = 0;
		dst_rect.y = 0;
		
		dst_rect.width  = kos->width;
		dst_rect.height = kos->height;
		
		src_rect.x = 0;
		src_rect.y = 0;
		
		src_rect.width  = kos->width  << 16;
		src_rect.height = kos->height << 16;
		
		dispman_display = vc_dispmanx_display_open(0);
		dispman_update  = vc_dispmanx_update_start(0);
		
		dispman_element = vc_dispmanx_element_add(dispman_update, dispman_display, 0, &dst_rect, 0, &src_rect, DISPMANX_PROTECTION_NONE, 0, 0, 0);
		native_window.element = dispman_element;
		
		native_window.width  = kos->width;
		native_window.height = kos->height;
		
		vc_dispmanx_update_submit_sync(dispman_update);
		
		kos->surface = eglCreateWindowSurface(kos->display, config, &native_window, NULL);
		assert(kos->surface != EGL_NO_SURFACE);
		
		result = eglMakeCurrent(kos->display, kos->surface, kos->surface, kos->context);
		assert(result != EGL_FALSE);
		
	}
	
	void bcm_ogles_exit(kos_t* kos) {
		glClear(GL_COLOR_BUFFER_BIT);
		eglSwapBuffers(kos->display, kos->surface);
		
		eglMakeCurrent   (kos->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		eglDestroySurface(kos->display, kos->surface);
		eglDestroyContext(kos->display, kos->context);
		eglTerminate     (kos->display);
		
	}
	
#endif
