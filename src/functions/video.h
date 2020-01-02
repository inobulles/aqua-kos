
uint64_t kos_video_width(uint64_t __zvm) {
	return video_width;
}

uint64_t kos_video_height(uint64_t __zvm) {
	return video_height;
}

uint64_t kos_video_fps(uint64_t __zvm) {
	return 60;
}

void kos_video_clear(uint64_t __zvm, uint64_t r, uint64_t g, uint64_t b, uint64_t a) {
	glClearColor((GLfloat) r / PRECISION, (GLfloat) g / PRECISION, (GLfloat) b / PRECISION, (GLfloat) a / PRECISION);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

extern void before_flip_devices(void);
extern void after_flip_devices(void);

void kos_video_flip(uint64_t __zvm) {
	before_flip_devices();
	video_flip();
	after_flip_devices();
}
