
void video_flip(void) {
	if (feature_video) SDL_GL_SwapWindow(sdl_window);
	else printf("[AQUA KOS] WARNING 'video_flip' will not work, because the video feature isn't enabled\n");
}
