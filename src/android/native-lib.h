
#ifndef NATIVE_LIB_H
#define NATIVE_LIB_H

#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#include "../lib/structs.h"

#if DYNAMIC_ES3
#include "gl3_stub.h"
#else

#if __ANDROID_API__ >= 24
#include <GLES3/gl32.h>
#elif __ANDROID_API__ >= 21
#include <GLES3/gl31.h>
#else
#include <GLES3/gl3.h>
#endif
#endif

extern char* LOG_TAG;
#define VERBOSE_OUTPUT 1
#include "alog.h"

#include "safemem.h"

#define MAX_PATH_LENGTH 4096
#define CALLBACK_NO_PARAMS "()V"

static JNIEnv* callback_env;
static jclass  callback_class;
static jobject callback_lib;

typedef struct {
	bool found;
	jmethodID method;

} callback_method_t;

static callback_method_t java_init_lib;

static callback_method_t java_new_font;
static callback_method_t java_font_remove;

static callback_method_t java_get_font_width;
static callback_method_t java_get_font_height;

static callback_method_t java_create_texture_from_font;

static callback_method_t java_read_external_slash_internal_storage_path;
static callback_method_t java_read_external_slash_internal_storage_path_bytes;

static callback_method_t java_package_exists;
static callback_method_t java_package_open;

static callback_method_t java_open_text_input;
static callback_method_t java_create_texture_from_screenshot;

static void init_callback_function(callback_method_t* __this, const char* name, const char* params) {
	__this->found = false;
	__this->method = callback_env->GetStaticMethodID(callback_class, name, params);

	if (__this->method == 0) {
		ALOGW("WARNING `%s` method could not be found\n", name);

	} else {
		__this->found = true;

	}

}

extern void nothing(...);

static unsigned char disable_gl = 0;

#define CALLBACK(               address, call_type, ...) (disable_gl ? 0         :                        (call_type)(callback_class, (&address)->method, __VA_ARGS__))
#define CALLBACK_VOID(          address,            ...) (disable_gl ? nothing() : callback_env->CallStaticVoidMethod(callback_class, (&address)->method, __VA_ARGS__))
#define CALLBACK_VOID_NO_PARAMS(address)                 (disable_gl ? nothing() : callback_env->CallStaticVoidMethod(callback_class, (&address)->method))
#define CALLBACK_INT(           address,            ...) (disable_gl ? 0         : callback_env->CallStaticIntMethod (callback_class, (&address)->method, __VA_ARGS__))

#define INTERNAL_STORAGE_PREFIX "/storage/emulated/0/"

#define SET_FINAL_PATH { \
    final_path = (char*) malloc(strlen(path) + strlen(INTERNAL_STORAGE_PREFIX) + 1); \
    strcpy(final_path, INTERNAL_STORAGE_PREFIX); \
    strcat(final_path, path); \
}

static bool load_asset_bytes(const char* path, char** buffer, unsigned long long* bytes) {
	if (buffer == NULL) {
		ALOGE("WARNING `buffer` is NULL\n");
		return true;

	}

	unsigned long long length = strlen(path);

	if (length > MAX_PATH_LENGTH) {
		ALOGV("WARNING Path length (%lld) is longer than MAX_PATH_LENGTH (%d)\n", length, MAX_PATH_LENGTH);
		return true;

	}

	extern bool default_assets;

	if (default_assets) {
		extern AAssetManager* asset_manager;
		AAsset* asset = AAssetManager_open(asset_manager, path, AASSET_MODE_UNKNOWN);
		
		if (asset) {
			assert(asset);

			*bytes  = (unsigned long long) AAsset_getLength(asset);
			*buffer = (char*)              malloc(*bytes + 1);

			AAsset_read (asset, *buffer, *bytes);
			AAsset_close(asset);

			(*buffer)[*bytes] = '\0';
			return false;

		} else {
			ALOGW("WARNING Could not load the file from `assets/%s`. Trying from internal / external storage ...\n", path);

		}

	}

	extern const char* internal_storage_path;
	extern bool is_internal_storage_path_set;

	char* final_path;
	SET_FINAL_PATH

	FILE* file = fopen(final_path, "rb");
	free(              final_path);

	if (file) {
		fseek(file, 0L, SEEK_END);
		*bytes = (unsigned long long) ftell(file);
		rewind(file);

		*buffer = (char*) malloc(    *bytes);
		fread(*buffer, sizeof(char), *bytes, file);
		fclose(file);

		return false;

	} else {
		if (!is_internal_storage_path_set) {
			ALOGW("WARNING Internal / external storage path could not be set\n");

		}

//		*bytes  = 0;
//		*buffer = NULL;

		return true; // problem

	}

}

static bool load_asset(const char* path, char** buffer) {
	unsigned long long bytes;
	return load_asset_bytes(path, buffer, &bytes);

}

extern bool check_gl_error(const char* function_name);
extern GLuint create_shader(GLenum shader_type, const char* code);
extern GLuint create_program(const char* vertex_code, const char* fragment_code);

class Renderer {
    public:
        virtual ~Renderer(void);

        void resize(int w, int h);
        unsigned long long render(void);

		virtual void draw_surface(surface_t* __this) = 0;

    protected:
        Renderer(void);

    private:
        unsigned long long step(void);
        unsigned long long last_frame_ns;

};

extern Renderer* create_es2_renderer();
extern Renderer* create_es3_renderer();

#endif
