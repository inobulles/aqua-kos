
#ifndef __AQUA__KOS_DEVICES_KTX_H
#define __AQUA__KOS_DEVICES_KTX_H

#ifdef __HAS_KTX
	#if KOS_USES_OPENGL_DESKTOP
		#define KTX_OPENGL 1
	#endif
	#if KOS_USES_OPENGLES
		#ifdef DYNAMIC_ES3
			#define KTX_OPENGL_ES2 1
		#else
			#define KTX_OPENGL_ES3 1
		#endif
	#endif

	#include "../external/libktx/ktx.h"
#endif

static void ktx_device_handle(unsigned long long** result, const char* __data) {
	char* data = (char*) __data;
	*result = &kos_bda_implementation.temp_value;
	
	#ifdef __HAS_KTX
		GLuint texture_id = 0;
		GLenum target;
		GLenum gl_error;
		GLboolean is_mipmapped;
		
		if (ktxLoadTextureM(data, 0 /* TODO size */, &texture_id, &target, NULL, &is_mipmapped, &gl_error, 0, NULL) != KTX_SUCCESS) {
			printf("WARNING Data is not KTX\n");
			kos_bda_implementation.temp_value = 0;
			return;
			
		}
		
		glEnable(target);
		
		if (is_mipmapped) glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		else              gl_texture_parameters();
		
		//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		kos_bda_implementation.temp_value = (unsigned long long) texture_id;
	#elif KOS_USES_JNI
	#endif
	
}

#endif
