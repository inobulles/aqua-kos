//
// Created by obiwac on 21.09.18.
//

#ifndef ANDROID_SAFEMEM_H
#define ANDROID_SAFEMEM_H

#include <stdlib.h>
#include <vector>

#define CPP_MMAN_STYLE true

static std::vector<void*> allocated_pointer_list;

static inline void* safe_malloc(unsigned long long bytes) {
	if (bytes > 4294967296 /* 1 << 32 */) {
		ALOGA("WARNING I'm pretty sure you shouldn't be allocating this much memory (%llu bytes)\n", bytes);
		
	}
	
#if CPP_MMAN_STYLE
	char* pointer = new char[bytes];
#else
	void* pointer = malloc(bytes);

	if (!pointer) {
		ALOGE("WARNING Could not allocate %lld bytes (malloc)\n", bytes);

	}
#endif

	allocated_pointer_list.push_back(pointer);
	return (void*) pointer;

}

static inline void safe_free(void* pointer) {
	long i;
	for (i = 0; i < allocated_pointer_list.size(); i++) {
		if (allocated_pointer_list.at((unsigned long) i) == pointer) {
			//ALOGE("REMOVED %ld (%p) FROM ALLOCATED_POINTER_VECTOR\n", i, pointer);
			allocated_pointer_list.erase(std::next(allocated_pointer_list.begin(), i));

#if CPP_MMAN_STYLE
			delete[] (char*) pointer;
#else
			if (!pointer) {
		ALOGE("WARNING Trying to free nullptr. Aborting ...\n");

	} else {
		free(pointer);

	}
#endif
			return;

		}

	}

	ALOGE("WARNING Value was never allocated (%s(%p))", __func__, pointer);

}

static inline void free_all_shit(void) {
	unsigned long long i;
	for (i = 0; i < allocated_pointer_list.size(); i++) {
		//ALOGE("ARE YOU CRAZY? YOU OUTTA YA MIND ??? %llu", (unsigned long long) allocated_pointer_list.at(i));
		delete[] (char*) allocated_pointer_list.at(i);

	}

	std::vector<void*>().swap(allocated_pointer_list);
	allocated_pointer_list.clear();
	allocated_pointer_list.shrink_to_fit();

}

#define malloc safe_malloc
#define free   safe_free

#endif //ANDROID_SAFEMEM_H
