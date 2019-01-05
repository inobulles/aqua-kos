
#ifndef __AQUA__SDL2_SRC_FUNCTIONS_VIDEO_H
	#define __AQUA__SDL2_SRC_FUNCTIONS_VIDEO_H
	
	#include "../macros_and_inclusions.h"
	#include "../lib/structs.h"
	
	#if KOS_USES_ANDROID
		#include "../android/root.h"
	#endif

	void video_clear(void) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
	}
	
	void video_clear_colour(unsigned long long r, unsigned long long g, unsigned long long b, unsigned long long a) {
		glClearColor((float) r / _UI64_MAX, (float) g / _UI64_MAX, (float) b / _UI64_MAX, (float) a / _UI64_MAX);
		
	}
	
	void video_draw(void) {
		printf("WARNING __this function (`video_draw`) is deprecated\n");
		
	}

	#if KOS_USES_JNI
		unsigned char waiting_video_flip = 0;
	#endif
	
	void __update_predefined_texture(unsigned long long name);
	static unsigned char predefined_textures_live = 0;
	
	static surface_t predefined_texture_surface_dummy;
	static unsigned char video_flip_called = 0;
	
	void video_flip(void) {
		video_flip_called = 1;
		
		if (current_video_flip_is_root_window) {
			if (load_program_overlay) {
				surface_set_alpha  ((unsigned long long) &predefined_texture_surface_dummy, _UI64_MAX >> 1);
				surface_set_layer  ((unsigned long long) &predefined_texture_surface_dummy, 256);
				surface_set_texture((unsigned long long) &predefined_texture_surface_dummy, load_program_overlay_texture);
				
				surface_draw       ((unsigned long long) &predefined_texture_surface_dummy);
				
				surface_set_texture((unsigned long long) &predefined_texture_surface_dummy,  0);
				surface_set_layer  ((unsigned long long) &predefined_texture_surface_dummy,  0);
				surface_set_alpha  ((unsigned long long) &predefined_texture_surface_dummy, -1);
				
			}
			
			#if KOS_USES_SDL2 && KOS_USES_OPENGL
				SDL_GL_SwapWindow(current_kos->window);
			#endif
			
			#if KOS_USES_BCM && KOS_USES_OPENGLES
				eglSwapBuffers(current_kos->display, current_kos->surface);
			#endif
			
			#if KOS_USES_JNI
				waiting_video_flip = 1;
			#endif
			
		} else {
			// non-root window flip
			
		}
		
		surface_layer_offset = 0.0f;
		
		#if KOS_3D_VISUALIZATION
			glRotatef(1.0f, 0.0f, 1.0f, 0.0f);
		#endif
		
		if (predefined_textures_live) {
			__update_predefined_texture(TEXTURE_BACKGROUND);
			__update_predefined_texture(TEXTURE_FROSTED_BACKGROUND);
			
		}
		
	}
	
	unsigned long long video_width(void) {
		if (current_video_flip_is_root_window) return (unsigned long long) current_kos->width;
		else                                   return (unsigned long long) load_program_overlay_dimensions[0];
		
	} unsigned long long video_height(void) {
		if (current_video_flip_is_root_window) return (unsigned long long) current_kos->height;
		else                                   return (unsigned long long) load_program_overlay_dimensions[1];
		
	}
	
	unsigned long long video_bpp(void) {
		return (unsigned long long) current_kos->bpp;
		
	}
	
	static unsigned long long kos_last_time;
	static unsigned long long kos_last_fps;
	
	unsigned long long video_fps(void) {
		current_video_flip_is_root_window
		
		#if KOS_USES_JNI
			extern unsigned long long gl_fps;
			kos_last_fps = gl_fps;
		#elif KOS_USES_SDL2
			unsigned long long tick_time = SDL_GetTicks();
			
			kos_last_fps  = (unsigned long long) (1000.0f / (float) (tick_time - kos_last_time));
			kos_last_time = tick_time;
		#endif
		
		return kos_last_fps;
		
	}
	
	void set_video_visibility(unsigned long long state) {
		KOS_DEPRECATED
		
		if (state == HIDDEN) {
			#if KOS_USES_SDL2
				SDL_MinimizeWindow(current_kos->window);
			#endif
			
		}
		
	}
	
	static unsigned char kos_is_mouse_pressed = 0;
	static unsigned char kos_has_clicked = 0;
	
	extern unsigned long long get_device_keyboard_key;
	extern unsigned long long get_device_keyboard_keycode;
	
	static unsigned long long resize_count;
	static unsigned char      first_event_flush = 1;

	#if KOS_USES_JNI
		static int event_pointer_x = 0;
		static int event_pointer_y = 0;

		static int event_pointer_click_type = 0;
		static int event_quit               = 0;

		static unsigned char has_the_event_been_updated_in_the_previous_call_to_Java_com_inobulles_obiwac_aqua_Lib_event_question_mark = 0;
		static int event_last_release = 1;
	#endif

	int gl_resize = 0;

	void get_events(unsigned long long ____this) { // I guess this shouldn't be here but idc tbh
		event_list_t* __this = (event_list_t*) ____this;
		
		unsigned long long half_width  = (unsigned long long) (current_kos->width  >> 1);
		unsigned long long half_height = (unsigned long long) (current_kos->height >> 1);
		
		__this->pointer_x          = half_width;
		__this->pointer_y          = half_height;
		__this->pointer_click_type = 0;
		
		__this->quit   = 0;
		__this->resize = 0;
		
		if (first_event_flush) {
			first_event_flush = 0;
			memset(__this, 0, sizeof(event_list_t));
			
		}
		
		if (current_video_flip_is_root_window && load_program_overlay) {
			return;
			
		}
		
		__this->pointer_click_type = kos_is_mouse_pressed;
		
		#if KOS_USES_SDL2
			SDL_Event event;
			SDL_PumpEvents();
			
			while (SDL_PollEvent(&event)) {
				if (event.type == SDL_QUIT) {
					__this->quit = 1;
					break;
					
				} else if (event.type == SDL_WINDOWEVENT) {
					switch (event.window.event) {
						case SDL_WINDOWEVENT_SIZE_CHANGED: {
							resize_count++;
							__this->resize = 1;
							
							current_kos->width  = event.window.data1;
							current_kos->height = event.window.data2;
							
							glViewport(0, 0, current_kos->width, current_kos->height);
							update_all_font_sizes();
							
							break;
							
						} default: {
							break;
							
						}
						
					}
					
					break;
					
				} else if (event.type == SDL_MOUSEMOTION) {
					__this->pointer_x = event.motion.x;
					__this->pointer_y = event.motion.y;
					
				} else if (event.type == SDL_MOUSEBUTTONDOWN) {
					__this->pointer_x = event.button.x;
					__this->pointer_y = event.button.y;
					
					kos_has_clicked = 1;
					kos_is_mouse_pressed = 1;
					
					break;
					
				} else if (event.type == SDL_MOUSEBUTTONUP) {
					__this->pointer_x = event.button.x;
					__this->pointer_y = event.button.y;
					
					kos_is_mouse_pressed = 0;
					break;
					
				} else if (event.type == SDL_KEYDOWN) {
					get_device_keyboard_key = event.key.keysym.scancode;
					break;
					
				} else if (event.type == SDL_TEXTINPUT) {
					get_device_keyboard_keycode = *event.text.text;
					break;
					
				}
				
			}
		#elif KOS_USES_JNI
			__this->quit   = (unsigned long long) event_quit;
			__this->resize = (unsigned long long) gl_resize;

			__this->pointer_click_type = (unsigned long long) !event_last_release;
			has_the_event_been_updated_in_the_previous_call_to_Java_com_inobulles_obiwac_aqua_Lib_event_question_mark = 0;
		
			__this->pointer_x = (unsigned long long) event_pointer_x;
			__this->pointer_y = (unsigned long long) event_pointer_y;
			
			gl_resize = 0;
		#endif

		__this->pointer_x = __this->pointer_x >= video_width()  ? half_width  : __this->pointer_x;
		__this->pointer_y = __this->pointer_y >= video_height() ? half_height : __this->pointer_y;
		
	}
	
	void free_events(event_list_t* __this) {
		
	}
	
	// predefined textures
	
	static texture_t predefined_background_texture;
	static texture_t predefined_frost_background_texture;
	
	static unsigned long long predefined_background_texture_dimensions      [2];
	static unsigned long long predefined_frost_background_texture_dimensions[2];
	
	// live predefined textures
	
	static unsigned long long* predefined_background_texture_data;
	static unsigned long long* predefined_frost_background_texture_data;
	
	static GLuint predefined_background_texture_shader;
	static GLuint predefined_frost_background_texture_shader;
	
	static GLuint predefined_background_texture_framebuffer;
	static GLuint predefined_frost_background_texture_framebuffer;
	
	void bmp_load(unsigned long long ____this, unsigned long long _path);
	void bmp_free(unsigned long long ____this);
	
	#define KOS_SUB2PEWDS KOS_USES_OPENGL_DESKTOP
	
	static int kos_setup_predefined_textures(kos_t* __this) {
		int warning = 0;
		
		#if KOS_SUB2PEWDS
			predefined_textures_live = 1;
			
			surface_new        ((unsigned long long) &predefined_texture_surface_dummy, -_UI64_MAX_MARGIN, -_UI64_MAX_MARGIN, _UI64_MAX_MARGIN << 1, _UI64_MAX_MARGIN << 1);
			surface_set_texture((unsigned long long) &predefined_texture_surface_dummy, 0);
			
			predefined_background_texture_dimensions      [0] = video_width();
			predefined_background_texture_dimensions      [1] = video_height();
			
			predefined_frost_background_texture_dimensions[0] = video_width();
			predefined_frost_background_texture_dimensions[1] = video_height();
			
			int bpp = 32;
			
			predefined_background_texture_data       = (unsigned long long*) malloc((bpp >> 3) * predefined_background_texture_dimensions      [0] * predefined_background_texture_dimensions      [1]);
			predefined_frost_background_texture_data = (unsigned long long*) malloc((bpp >> 3) * predefined_frost_background_texture_dimensions[0] * predefined_frost_background_texture_dimensions[1]);
			
			predefined_background_texture       = __texture_create(predefined_background_texture_data,       bpp, predefined_background_texture_dimensions      [0], predefined_background_texture_dimensions      [1], 0);
			predefined_frost_background_texture = __texture_create(predefined_frost_background_texture_data, bpp, predefined_frost_background_texture_dimensions[0], predefined_frost_background_texture_dimensions[1], 0);
			
			predefined_background_texture_framebuffer       = framebuffer_create(predefined_background_texture);
			predefined_frost_background_texture_framebuffer = framebuffer_create(predefined_frost_background_texture);
			
			warning += gl_load_shaders((GLuint*) &predefined_background_texture_shader,
			R"shader-code(
				#version 120
				
				void main(void) {
					gl_Position = ftransform();
					
				}
			)shader-code", R"shader-code(
				#version 120
				
				uniform sampler2D sampler_texture;
				uniform int       has_texture;
				uniform int       time;
				
				void main(void) {
					int time_ = time / 5;
					vec2 coord = gl_FragCoord.xy + vec2(time_ / 200.0f, time_ / 600.0f);
					
					float x = coord.x - coord.y / 2.0f;
					float y = coord.y + coord.x / 4.0f;
					
					float random = (sin((y + sin(x / 50.0f) * (100.0f + sin(time_ / 5000.0f) * 10.0f)) / 10.0f) + 1.0f) / 2.0f;
					
					if (random > sin(time_ / 3000.0f) / 6.0f + 0.4f) gl_FragColor = vec4(1.0f, 0.0f, 0.3f, 1.0f);
					else                                             gl_FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
					
				}
			)shader-code");
			
			warning += gl_load_shaders((GLuint*) &predefined_frost_background_texture_shader,
			R"shader-code(
				#version 120
				
				void main(void) {
					gl_Position = ftransform();
					
				}
			)shader-code", R"shader-code(
				#version 120
				
				uniform sampler2D sampler_texture;
				uniform int       has_texture;
				uniform int       time;
				
				void main(void) {
					int time_ = time / 5;
					vec2 coord = gl_FragCoord.xy + vec2(time_ / 200.0f, time_ / 600.0f);
					
					float x = coord.x - coord.y / 2.0f;
					float y = coord.y + coord.x / 4.0f;
					
					float random = (sin((y + sin(x / 50.0f) * (100.0f + sin(time_ / 5000.0f) * 10.0f)) / 10.0f) / 20.0f + 1.0f) / 2.0f;
					gl_FragColor = vec4(random * 1.0f, random * 0.0f, random * 0.3f, 1.0f);
					
				}
			)shader-code");
		#else
			bitmap_image_t temp_bmp;
			
			bmp_load((unsigned long long) &temp_bmp, (unsigned long long) "wallpaper/wallpaper.bmp");
			predefined_background_texture = texture_create((unsigned long long) temp_bmp.data, temp_bmp.bpp, temp_bmp.width, temp_bmp.height);
			
			predefined_background_texture_dimensions[0] = temp_bmp.width;
			predefined_background_texture_dimensions[1] = temp_bmp.height;
			
			bmp_free((unsigned long long) &temp_bmp);
			warning += predefined_background_texture == -1;
			
			bmp_load((unsigned long long) &temp_bmp, (unsigned long long) "wallpaper/frost.bmp");
			predefined_frost_background_texture = texture_create((unsigned long long) temp_bmp.data, temp_bmp.bpp, temp_bmp.width, temp_bmp.height);
			
			predefined_frost_background_texture_dimensions[0] = temp_bmp.width;
			predefined_frost_background_texture_dimensions[1] = temp_bmp.height;
			
			bmp_free((unsigned long long) &temp_bmp);
			warning += predefined_frost_background_texture == -1;
		#endif
		
		return warning;
		
	}
	
	static void kos_free_predefined_textures(kos_t* __this) {
		if (predefined_textures_live) {
			gl_delete_shader_program((GLuint*) &predefined_background_texture_shader);
			gl_delete_shader_program((GLuint*) &predefined_frost_background_texture_shader);
			
			framebuffer_remove(predefined_background_texture_framebuffer);
			framebuffer_remove(predefined_frost_background_texture_framebuffer);
			
			free(predefined_background_texture_data);
			free(predefined_frost_background_texture_data);
			
		}
		
		if (predefined_background_texture       != -1) texture_remove(predefined_background_texture);
		if (predefined_frost_background_texture != -1) texture_remove(predefined_frost_background_texture);
		
	}
	
	texture_t get_predefined_texture(unsigned long long name) {
		switch (name) {
			case TEXTURE_BACKGROUND:         return predefined_background_texture;
			case TEXTURE_FROSTED_BACKGROUND: return predefined_frost_background_texture;
			
			default: {
				printf("WARNING Texture %lld is unknown. Returning -1 ...\n", name);
				return (texture_t) -1;
				
			}
			
		}
		
	}
	
	static unsigned long long* get_predefined_texture_size(unsigned long long name) {
		switch (name) {
			case TEXTURE_BACKGROUND:         return predefined_background_texture_dimensions;
			case TEXTURE_FROSTED_BACKGROUND: return predefined_frost_background_texture_dimensions;
			
			default: {
				printf("WARNING Texture %lld is unknown. Returning 0 ...\n", name);
				return 0;
				
			}
			
		}
		
	}
	
	unsigned long long get_predefined_texture_width (unsigned long long name) { return get_predefined_texture_size(name)[0]; }
	unsigned long long get_predefined_texture_height(unsigned long long name) { return get_predefined_texture_size(name)[1]; }
	
	#define GET_PREDEFINED_TEXTURE_SIZES 1
	
	void __update_predefined_texture(unsigned long long name) {
		switch (name) {
			case TEXTURE_BACKGROUND: {
				if (predefined_textures_live) {
					GLuint default_shader = 0;
					
					framebuffer_bind(predefined_background_texture_framebuffer, 0, 0, video_width(), video_height());
					gl_use_shader_program((GLuint*) &predefined_background_texture_shader);
					
					video_clear();
					surface_draw((unsigned long long) &predefined_texture_surface_dummy);
					
					framebuffer_bind(0, 0, 0, video_width(), video_height());
					gl_use_shader_program((GLuint*) &default_shader);
					
				}
				
				break;
				
			} case TEXTURE_FROSTED_BACKGROUND: {
				if (predefined_textures_live) {
					GLuint default_shader = 0;
					
					framebuffer_bind(predefined_frost_background_texture_framebuffer, 0, 0, video_width(), video_height());
					gl_use_shader_program((GLuint*) &predefined_frost_background_texture_shader);
					
					video_clear();
					surface_draw((unsigned long long) &predefined_texture_surface_dummy);
					
					framebuffer_bind(0, 0, 0, video_width(), video_height());
					gl_use_shader_program((GLuint*) &default_shader);
					
				}
				
				break;
				
			} default: {
				printf("WARNING Texture %lld is unknown\n", name);
				break;
				
			}
			
		}
		
	}
	
	void update_predefined_texture(unsigned long long name) {
		
	}
	
	#include "screenshot.h"
	
#endif
