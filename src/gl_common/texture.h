
#ifndef __AQUA__SDL2_SRC_GL_COMMON_TEXTURE_H
	#define __AQUA__SDL2_SRC_GL_COMMON_TEXTURE_H
	
	#define TEXTURE_WRAP_TYPE GL_MIRRORED_REPEAT // pushes colour to absolute edge, GL_CLAMP_TO_EDGE to wrap colour around
	
	#include "../gl_versions/texture/gl_1.h"
	#include "../gl_versions/texture/gl_2.h"
	#include "../gl_versions/texture/gl_3.h"
	#include "../gl_versions/texture/gl_4.h"
	#include "../gl_versions/texture/gl_5.h"
	
	texture_t __texture_create(unsigned long long* data, unsigned long long bpp, unsigned long long width, unsigned long long height, unsigned char warning) {
		if (warning && ((((width & ~(width - 1)) == width) ? 0 : 1) || ((height & ~(height - 1)) == height ? 0 : 1))) {
			printf("WARNING The width (%llu) and the height (%llu) you have provided are not powers of 2\n", width, height);
			
		}
		
		switch (kos_best_gl_version_major) {
			case 1: return (texture_t) -1;
			case 2: return gl2_texture_create(data, bpp, width, height);
			case 3: return (texture_t) -1;
			case 4: return (texture_t) -1;
			case 5: return (texture_t) -1;
			
			default: {
				KOS_WARN_NO_GL_VERSION
				return (texture_t) -1;
				
			}
			
		}
		
	}
	
	texture_t texture_create(unsigned long long __data, unsigned long long bpp, unsigned long long width, unsigned long long height) {
		unsigned long long* data = (unsigned long long*) __data;
		return __texture_create(data, bpp, width, height, KOS_TEXTURE_WARNING);
		
	}
	
	void texture_remove(texture_t __this) {
		switch (kos_best_gl_version_major) {
			case 1: break;
			case 2: gl2_texture_remove(__this); break;
			case 3: break;
			case 4: break;
			case 5: break;
			
			default: {
				KOS_WARN_NO_GL_VERSION
				break;
				
			}
			
		}
		
	}
	
#endif
