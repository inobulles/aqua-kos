//
// Created by obiwac on 15.07.18.
//

#ifndef ANDROID_RENDERER_H
#define ANDROID_RENDERER_H

#include <string.h>
#include <stdlib.h>
#include <linux/time.h>
#include <time.h>
#include "native-lib.h"

Renderer::Renderer(void)  {
	last_frame_ns = 0;

}

Renderer::~Renderer(void) {}

void Renderer::resize(int w, int h) {
	last_frame_ns = 0;

	//if (!disable_gl) {
		glViewport(0, 0, w, h);
		ALOGA("RESIZE %d %d\n", w, h);

	//}

}

unsigned long long Renderer::step(void) {
	timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);

	unsigned long long nowNs = now.tv_sec * 1000000000ull + now.tv_nsec;
	unsigned long long fps = 0;

	if (last_frame_ns > 0) {
		float dt = float(nowNs - last_frame_ns) * 0.000000001f;
		fps = (unsigned long long) (1.0f / dt);

	}

	last_frame_ns = nowNs;
	return fps;

}

unsigned long long Renderer::render(void) {
	/*if (!disable_gl) {
		unsigned long long fps = step();
		check_gl_error("Renderer::render");
		return fps;

	} else {
		return 0;

	}*/

	return 0;

}

#endif
