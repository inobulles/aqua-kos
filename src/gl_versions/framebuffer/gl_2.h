
#ifndef __AQUA__SDL2_SRC_KOS_GL_VERSIONS_FRAMEBUFFER_GL_2_H
	#define __AQUA__SDL2_SRC_KOS_GL_VERSIONS_FRAMEBUFFER_GL_2_H
	
	#include "../../macros_and_inclusions.h"
	#include "../../gl_common/texture.h"

	#if !KOS_USES_JNI
		int glDeleteFramebuffers     (...);
		int glCheckFramebufferStatus (...);
		int glDrawBuffers            (...);
		int glFramebufferTexture     (...);
		int glFramebufferRenderbuffer(...);
		int glRenderbufferStorage    (...);
		int glBindRenderbuffer       (...);
		int glGenRenderbuffers       (...);
		int glBindFramebuffer        (...);
		int glGenFramebuffers        (...);
	#endif
	
	framebuffer_t gl2_framebuffer_create(texture_t texture) {
		#if KOS_USES_OPENGL_DESKTOP
			GLint                                  old_framebuffer;
			glGetIntegerv(GL_FRAMEBUFFER_BINDING, &old_framebuffer);

			GLuint framebuffer_id = 0;
			glGenFramebuffers(1, &framebuffer_id);
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);
			glBindFramebuffer   (GL_FRAMEBUFFER, old_framebuffer);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
				printf("WARNING Failed to create framebuffer\n");
				return 0;

			}

			return framebuffer_id;
		#else
			return -1;
		#endif
		
	}
	
	void gl2_framebuffer_bind(framebuffer_t __this, unsigned long long x, unsigned long long y, unsigned long long width, unsigned long long height) {
		glBindFramebuffer(GL_FRAMEBUFFER, __this);
		glViewport(x, y, width, height);
		
	}
	
	void gl2_framebuffer_remove(framebuffer_t __this) {
		#if KOS_USES_OPENGL_DESKTOP
			glDeleteFramebuffers(1, &__this);
		#endif
		
	}
	
#endif
