
#ifndef __AQUA__SDL2_SRC_FUNCTIONS_SYSTEM_H
	#define __AQUA__SDL2_SRC_FUNCTIONS_SYSTEM_H
	
	#include "../macros_and_inclusions.h"
	
	unsigned long long get_platform(void) {
		#if KOS_USES_JNI
			return PLATFORM_ANDROID;
		#else
			#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
				return PLATFORM_WIN32;
			#elif defined(_WIN64)
				return PLATFORM_WIN64;
			#elif defined(__CYGWIN__) && !defined(_WIN32)
				return PLATFORM_UNIX;
			#elif defined(__APPLE__) && defined(__MACH__)
				printf("WARNING Apple based platform detected, consider changing your OS\n");

				#if TARGET_IPHONE_SIMULATOR == 1 || TARGET_OS_IPHONE == 1
					return PLATFORM_IOS;
				#elif TARGET_OS_MAC == 1
					return PLATFORM_MACOS;
				#else
					return PLATFORM_CANCER;
				#endif
			#elif defined(BSD)
				printf("WARNING BSD based system detected. Returning Unix, as BSD is not included in the AQUA system standard\n");
				return PLATFORM_UNIX;
			#elif defined(__linux__)
				return PLATFORM_LINUX;
			#elif defined(__unix__)
				return PLATFORM_UNIX;
			#else
				return PLATFORM_OTHER;
			#endif
		#endif
		
	}
	
	unsigned long long platform_system(unsigned long long __command) {
		const char* command = (const char*) __command;
		
		#if SYSTEM_ACCESS
			return (unsigned long long) system(command);
		#else
			printf("WARNING You do not seem to have the privileges (SYSTEM_ACCESS = %d) to run the command `%s`\n", SYSTEM_ACCESS, command);
			return 1;
		#endif
		
	}
	
	void platform(void) {
		KOS_TODO
		
	}
	
	unsigned long long is_device_supported(unsigned long long __device) {
		const char* device = (const char*) __device;
		
		if      (strcmp(device, "texture")  == 0) return DEVICE_TEXTURE;
		else if (strcmp(device, "wm")       == 0) return DEVICE_WM;
		else if (strcmp(device, "math")     == 0) return DEVICE_MATH;
		else if (strcmp(device, "clock")    == 0) return DEVICE_CLOCK;
		else if (strcmp(device, "fbo")      == 0) return DEVICE_FBO;
		else if (strcmp(device, "shader")   == 0) return DEVICE_SHADER;
		else if (strcmp(device, "gl")       == 0) return DEVICE_GL;
		else if (strcmp(device, "gl batch") == 0) return DEVICE_GL_BATCH;
		else if (strcmp(device, "fs")       == 0) return DEVICE_FS;

		#if KOS_USES_JNI // JNI specific
			else if (strcmp(device, "android")         == 0) return DEVICE_ANDROID;
			else if (strcmp(device, "keyboard dialog") == 0) return DEVICE_KEYBOARD_DIALOG;
		#else // absolutely not JNI
			else if (strcmp(device, "keyboard") == 0) return DEVICE_KEYBOARD; /// TODO Add keyboard support for Android
		#endif
		
		// compute
		
		else if (strcmp(device, "nvcc") == 0 && !system("command -v nvcc")) return DEVICE_COMPUTE_CUDA_COMPILER;
		else if (strcmp(device, "cuda") == 0)                               return DEVICE_COMPUTE_CUDA_EXECUTOR;
		
		// extensions
		
		#ifdef __HAS_CURL
			else if (strcmp(device, "requests") == 0) return DEVICE_REQUESTS;
		#endif
		#ifdef __HAS_DISCORD
			else if (strcmp(device, "discord") == 0) return DEVICE_DISCORD;
		#endif
		
		else return DEVICE_NULL;
		
	}
	
	unsigned long long get_device_keyboard_key     = 0;
	unsigned long long get_device_keyboard_keycode = 0;
	
	#define KOS_DEVICE_COMMAND_WARNING(device_name) printf("WARNING The command you have passed to the " device_name " device (%s) is unrecognized\n", extra);
	
	typedef struct {
		uint64_t hour;
		uint64_t minute;
		uint64_t second;
		
		uint64_t day;
		uint64_t month;
		uint64_t year;
		
		uint64_t week_day;
		uint64_t year_day;
		
	} time_device_t;
	
	static struct tm* kos_tm_struct = (struct tm*) 0;
	static time_t     kos_time      = 0;
	
	typedef struct {
		char signature[sizeof(uint64_t)];
		uint64_t x;
		
	} math_device_generic_t;
	
	#define FLOAT_ONE 1000000
	
	#include <math.h>
	#include <time.h>
	
	#include "compute/cuda.h"
	
	#ifdef __HAS_CURL
		#include "requests.h"
	#endif
	#ifdef __HAS_DISCORD
		#include "discord.h"
	#endif
	
	typedef struct {
		signed long long x;
		signed long long y;
		signed long long z;
		
	} kos_gl_device_vertex_t;
	
	typedef struct { // quads
		unsigned long long pair1[2];
		unsigned long long pair2[2];
		unsigned long long pair3[2];
		unsigned long long pair4[2];
		
	} kos_gl_device_vertex_line_face_t;
	
	typedef struct {
		unsigned long long r;
		unsigned long long g;
		unsigned long long b;
		unsigned long long a;
		
	} kos_gl_device_colour_t;
	
	typedef struct {
		unsigned long long x;
		unsigned long long y;
		
	} kos_gl_device_texture_coord_t;
	
	typedef struct { GLfloat x; GLfloat y; GLfloat z;            } __attribute__((packed)) kos_gl_batch_device_batch_position_t;
	typedef struct { GLfloat r; GLfloat g; GLfloat b; GLfloat a; } __attribute__((packed)) kos_gl_batch_device_batch_colour_t;
	typedef struct { GLfloat x; GLfloat y;                       } __attribute__((packed)) kos_gl_batch_device_batch_texture_coord_t;
	
	typedef struct {
		unsigned char      has_texture;
		unsigned long long vertex_count;
		
		kos_gl_batch_device_batch_position_t*      positions;
		kos_gl_batch_device_batch_colour_t*        colours;
		kos_gl_batch_device_batch_texture_coord_t* texture_coords;
		
		unsigned long long index_count;
		uint32_t*          indices;
		
	} kos_gl_batch_device_batch_t;
	
	typedef struct {
		time_device_t previous_time_device;
		
		unsigned long long previous_fbo_device_create_result;
		unsigned long long previous_shader_device_create_result;
		
		unsigned long long previous_math_device_sqrt_result;
		unsigned long long previous_math_device_sin_result;
		unsigned long long previous_math_device_sigmoid_result;
		
		unsigned long long get_device_keyboard_key_packet;
		unsigned long long get_device_keyboard_keycode_packet;
		
		unsigned long long fs_device_result;

		#if KOS_USES_JNI
			unsigned long long previous_package_existence;
		#endif
		
	} kos_bda_extension_t;

	#define KOS_BDA_EXTENSION
	kos_bda_extension_t kos_bda_implementation;
	
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <dirent.h>

	#if KOS_USES_JNI
		static unsigned char text_input_has_response = 0;
	#endif

	int remove_directory_recursive(const char* name) {
		DIR* directory = opendir(name);
		
		if (directory == NULL) {
			return remove(name);
			
		}
		
		int errors = 0;
		
		struct dirent* entry;
		char path[PATH_MAX];
		
		while ((entry = readdir(directory)) != NULL) {
			if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
				snprintf(path, (size_t) PATH_MAX, "%s/%s", name, entry->d_name);
				
				if (entry->d_type == DT_DIR) errors += remove_directory_recursive(path);
				else                         errors += remove                    (path);
				
			}
			
		}
		
		closedir(directory);
		remove(name);
		
		return errors;
		
	}

	double exponential(double x) {
		int precision = 10;
		double sum = 1.0f;

		int i;
		for (i = precision - 1; i > 0; i--) {
			sum *= x;
			sum /= i;
			sum += 1;

		}

		return sum;

	}

	unsigned long long* get_device(unsigned long long device, unsigned long long __extra) {
		const char* extra = (const char*) __extra;
		unsigned long long* result = (unsigned long long*) 0;
		
		switch (device) {
			case DEVICE_MATH: {
				if (strcmp(extra, "sqrt") == 0) {
					math_device_generic_t* data = (math_device_generic_t*) extra;
					kos_bda_implementation.previous_math_device_sqrt_result = (unsigned long long) (sqrt((double) data->x / FLOAT_ONE) * FLOAT_ONE);
					result = &kos_bda_implementation.previous_math_device_sqrt_result;
					
				} else if (strcmp(extra, "sin") == 0) {
					math_device_generic_t* data = (math_device_generic_t*) extra;
					kos_bda_implementation.previous_math_device_sin_result = (unsigned long long) (sin((double) data->x / FLOAT_ONE) * FLOAT_ONE);
					result = &kos_bda_implementation.previous_math_device_sin_result;
					
				} else if (strcmp(extra, "sigmoid") == 0) {
					math_device_generic_t* data = (math_device_generic_t*) extra;
					double exp = exponential((double) ((int64_t) data->x) / FLOAT_ONE);
					kos_bda_implementation.previous_math_device_sigmoid_result = (unsigned long long) ((((exp / (exp + 1.0f)) - 0.5f) * 2.0f) * FLOAT_ONE);
					result = &kos_bda_implementation.previous_math_device_sigmoid_result;

				} else {
					KOS_DEVICE_COMMAND_WARNING("math")
					
				}
				
				break;
				
			} case DEVICE_KEYBOARD: {
				if (strcmp(extra, "press scancode") == 0) {
					kos_bda_implementation.get_device_keyboard_key_packet =                    get_device_keyboard_key;
					get_device_keyboard_key                          = 0;
					result                                           = &kos_bda_implementation.get_device_keyboard_key_packet;
					
				} else if (strcmp(extra, "press key") == 0) {
					kos_bda_implementation.get_device_keyboard_keycode_packet =                    get_device_keyboard_keycode;
					get_device_keyboard_keycode                          = 0;
					result                                               = &kos_bda_implementation.get_device_keyboard_keycode_packet;
					
				} else {
					KOS_DEVICE_COMMAND_WARNING("keyboard")
					
				}
				
				break;
				
			} case DEVICE_CLOCK: {
				kos_time = time(NULL);
				kos_tm_struct = localtime(&kos_time);
				
				if (strcmp(extra, "current") == 0) {
					kos_bda_implementation.previous_time_device.hour     = (uint64_t) kos_tm_struct->tm_hour;
					kos_bda_implementation.previous_time_device.minute   = (uint64_t) kos_tm_struct->tm_min;
					kos_bda_implementation.previous_time_device.second   = (uint64_t) kos_tm_struct->tm_sec;
					
					kos_bda_implementation.previous_time_device.day      = (uint64_t) kos_tm_struct->tm_mday;
					kos_bda_implementation.previous_time_device.month    = (uint64_t) kos_tm_struct->tm_mon;
					kos_bda_implementation.previous_time_device.year     = (uint64_t) kos_tm_struct->tm_year;
					
					kos_bda_implementation.previous_time_device.week_day = (uint64_t) kos_tm_struct->tm_wday;
					kos_bda_implementation.previous_time_device.year_day = (uint64_t) kos_tm_struct->tm_yday;
					
					result = (unsigned long long*) &kos_bda_implementation.previous_time_device;
					
				} else {
					KOS_DEVICE_COMMAND_WARNING("clock")
					
				}
				
				break;
				
			}
			
			case DEVICE_COMPUTE_CUDA_COMPILER: cuda_compile_bytecode(&result, extra); break;
			case DEVICE_COMPUTE_CUDA_EXECUTOR: cuda_execute_bytecode(&result, extra); break;
			
			case DEVICE_FS: {
				const unsigned long long* fs_command = (const unsigned long long*) extra;
				GET_PATH((char*) fs_command[1]);
				
				if (fs_command[0] == 'm') { // mkdir
					kos_bda_implementation.fs_device_result = (unsigned long long) mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
					result = &kos_bda_implementation.fs_device_result;
					
				} else if (fs_command[0] == 'r') { // remove
					kos_bda_implementation.fs_device_result = (unsigned long long) remove_directory_recursive(path);
					result = &kos_bda_implementation.fs_device_result;
					
				} else if (fs_command[0] == 'c') { // move
					GET_PATH_NAME(destination, (char*) fs_command[2]);
					
					kos_bda_implementation.fs_device_result = (unsigned long long) rename(path, destination);
					result = &kos_bda_implementation.fs_device_result;
					
				} else {
					KOS_DEVICE_COMMAND_WARNING("fs")
					
				}
				
				break;
				
			} case DEVICE_FBO: {
				const unsigned long long* fbo_command = (const unsigned long long*) extra;
				
				if (fbo_command[0] == 'c') { // create
					kos_bda_implementation.previous_fbo_device_create_result = framebuffer_create(fbo_command[1]);
					result = &kos_bda_implementation.previous_fbo_device_create_result;
					
				} else if (fbo_command[0] == 'b') {
					framebuffer_bind(fbo_command[1], fbo_command[4], fbo_command[5], fbo_command[2], fbo_command[3]);
					result = (unsigned long long*) 0;
					
				} else if (fbo_command[0] == 'r') { // remove
					framebuffer_remove(fbo_command[1]);
					result = (unsigned long long*) 0;
					
				} else {
					KOS_DEVICE_COMMAND_WARNING("fbo");
					
				}
				
				break;
				
			} case DEVICE_SHADER: {
				const unsigned long long* shader_command = (const unsigned long long*) extra;
				
				if (shader_command[0] == 'c') { // create
					kos_bda_implementation.previous_shader_device_create_result = (unsigned long long) gl_load_shaders((GLuint*) shader_command[1], (char*) shader_command[3], (char*) shader_command[4]);
					result = &kos_bda_implementation.previous_shader_device_create_result;
					
				} else if (shader_command[0] == 'u') { // use
					gl_use_shader_program((GLuint*) shader_command[1]);
					
				} else if (shader_command[0] == 'd') { // dispose
					gl_delete_shader_program((GLuint*) shader_command[1]);
					
				} else {
					KOS_DEVICE_COMMAND_WARNING("shader");
					
				}
				
				break;
				
			} case DEVICE_GL_BATCH: {
				#if !KOS_USES_SHADER_PIPELINE
					signed long long* batch_command     = (signed long long*)            extra;
					kos_gl_batch_device_batch_t* object = (kos_gl_batch_device_batch_t*) batch_command[1];

					if (batch_command[0] == 'p') { // draw
						glDisable(GL_CULL_FACE);

						glEnableClientState(GL_VERTEX_ARRAY);
						glEnableClientState(GL_COLOR_ARRAY);

						if (object->has_texture) {
							glEnableClientState(GL_TEXTURE_COORD_ARRAY);

						}

						glVertexPointer(3, GL_FLOAT, 0, object->positions);
						glColorPointer (4, GL_FLOAT, 0, object->colours);

						if (object->has_texture) {
							glTexCoordPointer(2, GL_FLOAT, 0, object->texture_coords);

						}

						glDrawElements(GL_TRIANGLES, object->index_count, GL_UNSIGNED_INT, object->indices);

						glDisableClientState(GL_VERTEX_ARRAY);
						glDisableClientState(GL_COLOR_ARRAY);

						if (object->has_texture) {
							glDisableClientState(GL_TEXTURE_COORD_ARRAY);

						}

					} else if (batch_command[0] == 'a') { // add
						unsigned long long old_vertex_count = object->vertex_count;
						unsigned long long old_index_count  = object->index_count;

						if (object->vertex_count == 0) {
							object->vertex_count = batch_command[3];
							object->index_count  = batch_command[9] * 3;

							object->positions      = (kos_gl_batch_device_batch_position_t*)      malloc(object->vertex_count * sizeof(kos_gl_batch_device_batch_position_t));
							object->colours        = (kos_gl_batch_device_batch_colour_t*)        malloc(object->vertex_count * sizeof(kos_gl_batch_device_batch_colour_t));
							object->texture_coords = (kos_gl_batch_device_batch_texture_coord_t*) malloc(object->vertex_count * sizeof(kos_gl_batch_device_batch_texture_coord_t));

							object->indices        = (uint32_t*)                                  malloc(object->index_count  * sizeof(uint32_t));

						} else {
							object->vertex_count += batch_command[3];
							object->index_count  += batch_command[9] * 3;

							object->positions      = (kos_gl_batch_device_batch_position_t*)      realloc(object->positions,      object->vertex_count * sizeof(kos_gl_batch_device_batch_position_t));
							object->colours        = (kos_gl_batch_device_batch_colour_t*)        realloc(object->colours,        object->vertex_count * sizeof(kos_gl_batch_device_batch_colour_t));
							object->texture_coords = (kos_gl_batch_device_batch_texture_coord_t*) realloc(object->texture_coords, object->vertex_count * sizeof(kos_gl_batch_device_batch_texture_coord_t));

							object->indices        = (uint32_t*)                                  realloc(object->indices,        object->index_count  * sizeof(uint32_t));

						}

						kos_gl_device_vertex_line_face_t* faces = (kos_gl_device_vertex_line_face_t*) batch_command[8];

						kos_gl_device_vertex_t*        positions      = (kos_gl_device_vertex_t*)        batch_command[2];
						kos_gl_device_colour_t*        colours        = (kos_gl_device_colour_t*)        batch_command[4];
						kos_gl_device_texture_coord_t* texture_coords = (kos_gl_device_texture_coord_t*) batch_command[6];

						unsigned long long i;
						for (i = old_vertex_count; i < object->vertex_count; i++) {
							unsigned long long absolute = i - old_vertex_count;

							object->positions[i].x = (GLfloat) positions[absolute].x / FLOAT_ONE;
							object->positions[i].y = (GLfloat) positions[absolute].y / FLOAT_ONE;
							object->positions[i].z = (GLfloat) positions[absolute].z / FLOAT_ONE;

							if (batch_command[5]) {
								object->colours[i].r = (GLfloat) colours[absolute].r / FLOAT_ONE;
								object->colours[i].g = (GLfloat) colours[absolute].g / FLOAT_ONE;
								object->colours[i].b = (GLfloat) colours[absolute].b / FLOAT_ONE;
								object->colours[i].a = (GLfloat) colours[absolute].a / FLOAT_ONE;

							} else {
								object->colours[i].r = 1.0f;
								object->colours[i].g = 1.0f;
								object->colours[i].b = 1.0f;
								object->colours[i].a = 1.0f;

							} if (batch_command[7]) {
								object->has_texture      = 1;
								unsigned long long index = absolute;

								if      (absolute % 3 == 0) index = faces[absolute / 3].pair1[1];
								else if (absolute % 3 == 1) index = faces[absolute / 3].pair2[1];
								else                        index = faces[absolute / 3].pair3[1];

								object->texture_coords[i].x = (GLfloat) texture_coords[index].x / FLOAT_ONE;
								object->texture_coords[i].y = (GLfloat) texture_coords[index].y / FLOAT_ONE;

							} else {
								object->texture_coords[i].x = 0.0f;
								object->texture_coords[i].y = 0.0f;

							}

						}

						for (i = old_index_count / 3; i < object->index_count / 3; i++) {
							unsigned long long absolute = i - old_index_count;

							object->indices[i * 3    ] = faces[i].pair1[0];
							object->indices[i * 3 + 1] = faces[i].pair2[0];
							object->indices[i * 3 + 2] = faces[i].pair3[0];

						}

					} else if (batch_command[0] == 'c') { // create
						object = (kos_gl_batch_device_batch_t*) malloc(sizeof(kos_gl_batch_device_batch_t));
						batch_command[1] = (signed long long) object;

						object->vertex_count = 0;
						object->index_count  = 0;
						object->has_texture  = 0;

					} else if (batch_command[0] == 'd') { // dispose
						if (object->vertex_count) {
							free(object->positions);
							free(object->colours);
							free(object->texture_coords);

						} if (object->index_count) {
							free(object->indices);

						}

						free(object);

					} else {
						KOS_DEVICE_COMMAND_WARNING("gl batch");

					}
				#else
					KOS_DEVICE_COMMAND_WARNING("gl batch");
				#endif
				
				break;
				
			} case DEVICE_GL: {
				const signed long long* gl_command = (const signed long long*) extra;

				#if !KOS_USES_SHADER_PIPELINE
					/*if (gl_command[0] == 'b') { // batch commands
						if (gl_command[25] == 'e') { // draw elements
							kos_gl_device_vertex_line_face_t* faces = (kos_gl_device_vertex_line_face_t*) gl_command[17];
							unsigned long long                count = (unsigned long long)                gl_command[18];

							uint32_t* int_indices = (uint32_t*) malloc(count * 3 * sizeof(uint32_t));

							int i;
							for (i = 0; i < count; i++) {
								int_indices[i * 3    ] = faces[i].pair1[0];
								int_indices[i * 3 + 1] = faces[i].pair2[0];
								int_indices[i * 3 + 2] = faces[i].pair3[0];

							}

							glDrawElements(GL_TRIANGLES, count * 3, GL_UNSIGNED_INT, int_indices);
							free(int_indices);

						} else if (gl_command[25] == 's') { // enable/disable client state
							void (*client_state_function)(GLenum cap) = gl_command[10] ? glEnableClientState : glDisableClientState;

							if (gl_command[7]) client_state_function(GL_VERTEX_ARRAY);
							if (gl_command[8]) client_state_function(GL_COLOR_ARRAY);
							if (gl_command[9]) client_state_function(GL_TEXTURE_COORD_ARRAY);

						} else if (gl_command[25] == 'p') { // set pointer
							if (gl_command[7]) {
								unsigned long long count = gl_command[12];
								GLfloat* pointer = (GLfloat*) malloc(count * 3 * sizeof(GLfloat));
								kos_gl_device_vertex_t* vertices = (kos_gl_device_vertex_t*) gl_command[11];

								int i;
								for (i = 0; i < count; i++) {
									pointer[i * 3    ] = (double) vertices[i].x / FLOAT_ONE;
									pointer[i * 3 + 1] = (double) vertices[i].y / FLOAT_ONE;
									pointer[i * 3 + 2] = (double) vertices[i].z / FLOAT_ONE;

								}

								glVertexPointer(3, GL_FLOAT, 0, pointer);
								free(pointer);

							} if (gl_command[8]) {
								unsigned long long count = gl_command[20];
								GLfloat* pointer = (GLfloat*) malloc(count * 4 * sizeof(GLfloat));
								kos_gl_device_colour_t* colours = (kos_gl_device_colour_t*) gl_command[19];

								int i;
								for (i = 0; i < count; i++) {
									pointer[i * 4    ] = (double) colours[i].red   / FLOAT_ONE;
									pointer[i * 4 + 1] = (double) colours[i].green / FLOAT_ONE;
									pointer[i * 4 + 2] = (double) colours[i].blue  / FLOAT_ONE;
									pointer[i * 4 + 3] = (double) colours[i].alpha / FLOAT_ONE;

								}

								glColorPointer(4, GL_FLOAT, 0, pointer);
								free(pointer);

							} if (gl_command[9]) {
								unsigned long long count = gl_command[22];
								GLfloat* pointer = (GLfloat*) malloc(count * 2 * sizeof(GLfloat));
								kos_gl_device_texture_coord_t* texture_coords = (kos_gl_device_texture_coord_t*) gl_command[21];

								int i;
								for (i = 0; i < count; i++) {
									pointer[i * 2    ] = (double) texture_coords[i].x / FLOAT_ONE;
									pointer[i * 2 + 1] = (double) texture_coords[i].y / FLOAT_ONE;

								}

								glTexCoordPointer(2, GL_FLOAT, 0, pointer);
								free(pointer);

							}

						}

					} else */if (gl_command[0] == 't') { // bind/activate texture
						glActiveTexture(GL_TEXTURE0 +  gl_command[23]);
						glBindTexture  (GL_TEXTURE_2D, gl_command[24]);

					} else if (gl_command[0] == 'f') { // frustum
						glMatrixMode(GL_PROJECTION);
						glLoadIdentity();

						glFrustum( \
							(double) gl_command[1] / FLOAT_ONE, \
							(double) gl_command[2] / FLOAT_ONE, \
							(double) gl_command[3] / FLOAT_ONE, \
							(double) gl_command[4] / FLOAT_ONE, \
							(double) gl_command[5] / FLOAT_ONE, \
							(double) gl_command[6] / FLOAT_ONE  \
						);

						glTranslatef(0.0f, 0.0f, -3.0f);

						glMatrixMode(GL_MODELVIEW);
						glLoadIdentity();

					} else if (gl_command[0] == 'o') { // ortho
						glMatrixMode(GL_PROJECTION);
						glLoadIdentity();

						glEnable(GL_CULL_FACE);
						glCullFace(GL_BACK);
						glFrontFace(GL_CCW);

						glOrtho( \
							(double) gl_command[1] / FLOAT_ONE, \
							(double) gl_command[2] / FLOAT_ONE, \
							(double) gl_command[3] / FLOAT_ONE, \
							(double) gl_command[4] / FLOAT_ONE, \
							(double) gl_command[5] / FLOAT_ONE, \
							(double) gl_command[6] / FLOAT_ONE  \
						);

						glTranslatef(0.0f, 0.0f, -100.0f);

						glMatrixMode(GL_MODELVIEW);
						glLoadIdentity();

					} else if (gl_command[0] == 'c') { // colour
						glColor4f( \
							(double) gl_command[13] / FLOAT_ONE, \
							(double) gl_command[14] / FLOAT_ONE, \
							(double) gl_command[15] / FLOAT_ONE, \
							(double) gl_command[16] / FLOAT_ONE  \
						);

					} else if (gl_command[0] == 'v') { // draw
						surface_t test;
						surface_new(&test, -_UI64_MAX_MARGIN, -_UI64_MAX_MARGIN, 0, 0);
						surface_set_texture(&test, 0);
						surface_draw(&test);

						glPointSize(4.0f);
						glBegin(GL_POINTS);

						kos_gl_device_vertex_t*           vertices = (kos_gl_device_vertex_t*)           gl_command[11];
						kos_gl_device_vertex_line_face_t* faces    = (kos_gl_device_vertex_line_face_t*) gl_command[17];

						unsigned long long i;
						for (i = 0; i < gl_command[12]; i++) {
							glVertex3f((double) vertices[i].x / FLOAT_ONE, -(double) vertices[i].y / FLOAT_ONE, (double) vertices[i].z / FLOAT_ONE);

						}

						glEnd();
						glBegin(GL_LINES);

						for (i = 0; i < gl_command[18]; i++) {
							glVertex3f((double) vertices[faces[i].pair1[0]].x / FLOAT_ONE, -(double) vertices[faces[i].pair1[0]].y / FLOAT_ONE, (double) vertices[faces[i].pair1[0]].z / FLOAT_ONE);
							glVertex3f((double) vertices[faces[i].pair2[0]].x / FLOAT_ONE, -(double) vertices[faces[i].pair2[0]].y / FLOAT_ONE, (double) vertices[faces[i].pair2[0]].z / FLOAT_ONE);

							glVertex3f((double) vertices[faces[i].pair2[0]].x / FLOAT_ONE, -(double) vertices[faces[i].pair2[0]].y / FLOAT_ONE, (double) vertices[faces[i].pair2[0]].z / FLOAT_ONE);
							glVertex3f((double) vertices[faces[i].pair3[0]].x / FLOAT_ONE, -(double) vertices[faces[i].pair3[0]].y / FLOAT_ONE, (double) vertices[faces[i].pair3[0]].z / FLOAT_ONE);

							glVertex3f((double) vertices[faces[i].pair3[0]].x / FLOAT_ONE, -(double) vertices[faces[i].pair3[0]].y / FLOAT_ONE, (double) vertices[faces[i].pair3[0]].z / FLOAT_ONE);

							if (faces[i].pair4[0]) {
								glVertex3f((double) vertices[faces[i].pair4[0]].x / FLOAT_ONE, -(double) vertices[faces[i].pair4[0]].y / FLOAT_ONE, (double) vertices[faces[i].pair4[0]].z / FLOAT_ONE);
								glVertex3f((double) vertices[faces[i].pair4[0]].x / FLOAT_ONE, -(double) vertices[faces[i].pair4[0]].y / FLOAT_ONE, (double) vertices[faces[i].pair4[0]].z / FLOAT_ONE);

							}

							glVertex3f((double) vertices[faces[i].pair1[0]].x / FLOAT_ONE, -(double) vertices[faces[i].pair1[0]].y / FLOAT_ONE, (double) vertices[faces[i].pair1[0]].z / FLOAT_ONE);

						}

						glEnd();


					} else if (gl_command[0] == 'r') { // rotate
						glRotatef( \
							(double) gl_command[7]  / FLOAT_ONE, \
							(double) gl_command[8]  / FLOAT_ONE, \
							(double) gl_command[9]  / FLOAT_ONE, \
							(double) gl_command[10] / FLOAT_ONE  \
						);

					} else {
						KOS_DEVICE_COMMAND_WARNING("gl");

					}
				#else
					KOS_DEVICE_COMMAND_WARNING("gl");
				#endif
				
				break;
				
			}

			#if KOS_USES_JNI
				case DEVICE_ANDROID: {
					if (extra[0] == 'p' && extra[1] == 'k' && extra[2] == 'g' && extra[3] == 'e') {
						extra += 4;
						kos_bda_implementation.previous_package_existence = (unsigned long long) CALLBACK_INT(java_package_exists, callback_env->NewStringUTF(extra));
						result = &kos_bda_implementation.previous_package_existence;

					} else {
						KOS_DEVICE_COMMAND_WARNING("android")

					}

					break;

				}
			#endif

			case DEVICE_KEYBOARD_DIALOG: {
				#if KOS_USES_JNI
					if (strcmp(extra, "open") == 0) {
						CALLBACK_VOID_NO_PARAMS(java_open_text_input);
						text_input_has_response = 0;

						extern bool cw_pause;
						cw_pause = true;

					} else
				#endif

				{ KOS_DEVICE_COMMAND_WARNING("keyboard dialog") }
				break;

			} case DEVICE_NULL: {
				printf("WARNING The device you have selected is DEVICE_NULL\n");
				break;
				
			} default: {
				printf("WARNING Device %lld does not seem to exist or doesn't accept `get` commands\n", device);
				break;
				
			}
			
		}
		
		return result;
		
	}
	
	#ifdef __HAS_CURL
		typedef struct {
			kos_request_response_t request_response;
			unsigned long long     pointer_to_const_url;
			
		} request_device_struct_t;
	#endif
	#ifdef __HAS_DISCORD
		typedef struct {
			kos_discord_rpc_t discord_rpc;
			
		} discord_device_struct_t;
	#endif
	
	void send_device(unsigned long long device, unsigned long long __extra, unsigned long long __data) {
		const char*         extra = (const char*)        __extra;
		unsigned long long* data  = (unsigned long long*) __data;
		
		switch (device) {
			case DEVICE_TEXTURE: {
				if (strcmp(extra, "sharp") == 0) SHARP_TEXTURES = *data;
				else KOS_DEVICE_COMMAND_WARNING("texture")
				
				break;
				
			} case DEVICE_WM: {
				if (strcmp(extra, "visible") == 0 && *data == HIDDEN) {
					#if KOS_USES_SDL2
						SDL_MinimizeWindow(current_kos->window);
					#endif
					
				} else {
					KOS_DEVICE_COMMAND_WARNING("wm")
					
				}
				
				break;
				
			}
			#ifdef __HAS_CURL
				case DEVICE_REQUESTS: {
					request_device_struct_t* request_device_struct = (request_device_struct_t*) data;
					
					if      (strcmp(extra, "get")  == 0) kos_requests_get (&request_device_struct->request_response, (const char*) request_device_struct->pointer_to_const_url);
					else if (strcmp(extra, "free") == 0) kos_requests_free(&request_device_struct->request_response);
					else KOS_DEVICE_COMMAND_WARNING("requests")
					
					break;
					
				}
			#endif
			#ifdef __HAS_DISCORD
				case DEVICE_DISCORD: {
					discord_device_struct_t* discord_device_struct = (discord_device_struct_t*) data;
					
					if      (strcmp(extra, "rpc init")    == 0) init_discord_rpc  ((unsigned long long) data);
					else if (strcmp(extra, "rpc loop")    == 0) loop_discord_rpc  (&discord_device_struct->discord_rpc);
					else if (strcmp(extra, "rpc update")  == 0) update_discord_rpc(&discord_device_struct->discord_rpc);
					else if (strcmp(extra, "rpc dispose") == 0) dispose_discord_rpc();
					else KOS_DEVICE_COMMAND_WARNING("requests")
					
					break;
					
				}
			#endif
			case DEVICE_NULL: {
				printf("WARNING The device you have selected is DEVICE_NULL\n");
				break;
				
			} default: {
				printf("WARNING Device %lld does not seem to exist or doesn't accept `send` commands\n", device);
				break;
				
			}
			
		}
		
	}
	
#endif
