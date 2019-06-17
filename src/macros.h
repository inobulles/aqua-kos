
#ifndef __AQUA__SDL2_SRC_MACROS_AND_INCLUSIONS_H
	#define __AQUA__SDL2_SRC_MACROS_AND_INCLUSIONS_H

	#if KOS_USES_JNI
		#include "platforms/android/alog.h"
		static bool load_asset_bytes(const char* path, char** buffer, unsigned long long* bytes);
	#endif

	#include "macros/inclusions.h"
	#include "macros/structs.h"
	#include "macros/global_variables.h"
	#include "macros/macros.h"
	#include "macros/warnings.h"
	#include "macros/prototypes.h"
	#include "macros/predefined_textures.h"
	
#endif
