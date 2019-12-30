
void video_flip(void) {
	eglSwapBuffers(egl_display, egl_surface);
}
