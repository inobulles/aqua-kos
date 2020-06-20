
void video_flip(void) {
	if (feature_video) eglSwapBuffers(egl_display, egl_surface);
	else printf("[AQUA KOS] WARNING 'video_flip' will not work, because the video feature isn't enabled\n");
}
