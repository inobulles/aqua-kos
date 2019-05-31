
#ifndef __AQUA__SDL2_SRC_GL_COMMON_FRAMEBUFFER_H
	#define __AQUA__SDL2_SRC_GL_COMMON_FRAMEBUFFER_H
	
	typedef unsigned long long framebuffer_t;
	
	unsigned long long video_width (void);
	unsigned long long video_height(void);
	unsigned long long video_bpp   (void);
	
	#include "../gl_versions/framebuffer/gl_1.h"
	#include "../gl_versions/framebuffer/gl_2.h"
	#include "../gl_versions/framebuffer/gl_3.h"
	#include "../gl_versions/framebuffer/gl_4.h"
	#include "../gl_versions/framebuffer/gl_5.h"
	
	framebuffer_t framebuffer_create(texture_t texture) {
		switch (kos_best_gl_version_major) {
			case 1: return -1;
			case 2: return gl2_framebuffer_create(texture);
			case 3: return -1;
			case 4: return -1;
			case 5: return -1;
			
			default: {
				KOS_WARN_NO_GL_VERSION
				return -1;
				
			}
			
		}
		
	}
	
	void framebuffer_bind(framebuffer_t __this, unsigned long long x, unsigned long long y, unsigned long long width, unsigned long long height) {
		switch (kos_best_gl_version_major) {
			case 1: break;
			case 2: gl2_framebuffer_bind(__this, x, y, width, height); break;
			case 3: break;
			case 4: break;
			case 5: break;
			
			default: {
				KOS_WARN_NO_GL_VERSION
				break;
				
			}
			
		}
		
	}
	
	void framebuffer_remove(framebuffer_t __this) {
		switch (kos_best_gl_version_major) {
			case 1: break;
			case 2: gl2_framebuffer_remove(__this); break;
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
