
#ifndef __AQUA__ANDROID_KOS_HEAP_H
#define __AQUA__ANDROID_KOS_HEAP_H

// HEAP.h

#ifndef ALOGE
#define ALOGE(...) printf(__VA_ARGS__)
#endif

#include "heap_size.h"

typedef struct {
	unsigned long number;
	unsigned long set;
	unsigned long position;

} frame_t;

typedef struct {
	frame_t start;
	unsigned long length;

} frame_set_t;

void init_heap(void);

frame_t     heap_get_frame(unsigned long id);
frame_t     heap_first_available_frame(void);
frame_set_t heap_first_available_frame_set(unsigned long length);

void        heap_ffree(frame_t frame);
frame_t     heap_falloc(void);
frame_set_t heap_fsalloc(unsigned long length);

void  heap_mfree(unsigned long long pointer, unsigned long long bytes);
void* heap_malloc                           (unsigned long long bytes);

// HEAP.c

unsigned char heap_space[MAX_HEAP_SPACE];
unsigned char frames    [MAX_FRAMES];

static frame_t     frame_buffer;
static frame_set_t frame_set_buffer;

static unsigned long free_buffer;

void init_heap(void) {
	unsigned long long i;
	for (i = 0; i < MAX_FRAMES / sizeof(unsigned long long); i++) {
		((unsigned long long*) frames)[i] = 0;

	}

}

static inline void heap_set_frame_buffer(frame_t* __this, unsigned long long frame, unsigned long long position) {
	__this->set      = frame;
	__this->position =             position;
	__this->number   = frame * 8 + position;

}

frame_t heap_first_available_frame(void) {
	unsigned long long frame;
	unsigned long long position;

	for (frame = 0; frame < MAX_FRAMES; frame++) {
		for (position = 0; position < 8; position++) {
			if (!(frames[frame] >> position & 0b1)) {
				heap_set_frame_buffer(&frame_buffer, frame, position);
				return frame_buffer;

			}

		}

	}

	ALOGE("WARNING The heap ran out of memory\n");
	return frame_buffer;

}

frame_set_t heap_first_available_frame_set(unsigned long length) {
	unsigned long frame_count = 0;

	unsigned long long frame;
	unsigned long long position;

	for (frame = 0; frame < MAX_FRAMES; frame++) {
		for (position = 0; position < 8; position++) {
			if (!(frames[frame] >> position & 0b1)) {
				if (frame_count == 0) {
					heap_set_frame_buffer(&frame_buffer, frame, position);

				}

				frame_count++;

			} else {
				frame_count = 0;

			}

			if (frame_count >= length) {
				frame_set_buffer.start  = frame_buffer;
				frame_set_buffer.length = frame_count;

				return frame_set_buffer;

			}

		}

	}

	ALOGE("WARNING The heap doesn't have enough consecutive frames to fit a variable (var_bytes = ~0x%lx)\n", length * 16);
	return frame_set_buffer;

}

frame_t heap_get_frame(unsigned long id) {
	frame_t frame = {
		.number   = id,
		.set      = id / 8,
		.position = id % 8,
	};

	return frame;

}

frame_t heap_falloc(void) {
	frame_t frame     = heap_first_available_frame();
	frames [frame.set] = (unsigned char) (frames[frame.set] | (1 << frame.position));

	frame_t result;

	result.number   = frame.number;
	result.set      = frame.set;
	result.position = frame.position;

	return result;

}

void heap_ffree(frame_t frame) {
	if (frame.set < MAX_FRAMES) {
		frames[frame.set] &= ~(1 << frame.position);

	}

}

frame_set_t heap_fsalloc(unsigned long length) {
	frame_set_t frame_set = heap_first_available_frame_set(length);
	frame_t     buffer;

	unsigned long long i;
	for (i = 0; i < frame_set.length; i++) {
		buffer = heap_get_frame(i + frame_set.start.number);
		frames[buffer.set] |= 1 << buffer.position;

	}

	frame_set_t result;

	result.start  = heap_get_frame(frame_set.start.number);
	result.length = length;

	return result;

}

#define SYSTEM_MEMORY 0 /// THIS SHOULD ALWAYS BE 0

void* heap_malloc(unsigned long long bytes) {
#if SYSTEM_MEMORY
	return safe_malloc(bytes);
#else
	frame_set_buffer = heap_fsalloc((unsigned long) bytes / 0x10 + 1);
	return &heap_space[frame_set_buffer.start.number * 0x10];
#endif

}

void heap_mfree(unsigned long long pointer, unsigned long long bytes) {
#if SYSTEM_MEMORY
	safe_free((void*) pointer);
#else
	free_buffer  = (unsigned long) pointer - (unsigned long) &heap_space;
	free_buffer /= 0x10;

	int i;
	for (i = 0; i < (unsigned long) bytes / 0x10 + 1; i++) {
		frame_buffer = heap_get_frame(i + free_buffer);
		heap_ffree(frame_buffer);

	}
#endif

}

#endif
