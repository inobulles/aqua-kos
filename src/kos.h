
#ifndef __AQUA__SDL2_SRC_KOS_H
	#define __AQUA__SDL2_SRC_KOS_H
	
	#define KOS_3D_VISUALIZATION 0
	
	#include "macros.h"
	
	#if KOS_USES_BCM
		#include "bcm/bcm.h"
	#endif

	#if KOS_USES_ANDROID
		#include "android/root.h"
	#endif
	
	#ifdef __HAS_X11
		#include "platforms/xwm/xwm.h"
	#endif
	
	#include "functions/system.h"
	static unsigned char has_to_close_xwm = 0;
	
	void kos_quit(kos_t* __this) {
		#ifdef __HAS_X11
			if (has_to_close_xwm) {
				close_xwm();
				
			}
		#endif
		
		kos_close_joysticks();
		
		if (!__this->text_only) {
			kos_free_predefined_textures(__this);
			kos_destroy_fonts();
			
			#if KOS_USES_OPENGLES
				#if KOS_USES_BCM
					bcm_ogles_exit(__this);
				#endif
				
				#if KOS_USES_JNI
					if (eglGetCurrentContext() == __this->context) {
						gl_delete_shader_program(&__this->shader_program);
						
						free(__this->vertex_shader);
						free(__this->fragment_shader);
						
					}
				#endif
			#endif
			
		}
		
		#if KOS_USES_SDL2
			if (!__this->text_only) {
				#if KOS_USES_OPENGL
					SDL_GL_DeleteContext(__this->context);
				#endif
				SDL_DestroyWindow(__this->window);
				
			}
			
			SDL_Quit();
			printf("Destroyed all SDL subsystems\n");
		#endif
		
		#ifdef __HAS_AUDIO
			printf("Freeing audio ...\n");
			
			#if !KOS_USES_JNI
				pulse_free();
			#endif
		#endif
		
		#ifdef __HAS_CURL
			printf("Freeing requests and CURL ...\n");
			request_global_free();
		#endif
		
	}
	
	int kos_init(kos_t* __this) {
		has_to_close_xwm = 0;
		
		#ifdef __HAS_X11
			printf("INFO KOS has X11, will now try to open a new XWM ...\n");
			
			if (1 /*open_xwm()*/) {
				printf("WARNING Failed to launch XWM\n");
				
			} else {
				has_to_close_xwm = 1;
				
			}
		#endif
		
		#ifdef __HAS_CURL
			printf("INFO KOS has CURL, the `requests` device will thus be enabled\n");
		#endif
		#ifdef __HAS_DISCORD
			printf("INFO KOS has Discord, the `discord` device will thus be enabled\n");
		#endif
		
		current_kos = __this;
		
		if (!__this->text_only) {
			#if KOS_USES_SDL2
				__this->window  = NULL;
				__this->context = NULL;
				
				if (SDL_Init(SDL_INIT_VIDEO) < 0) {
					printf("ERROR SDL2 could not initialize video (%s)\n", SDL_GetError());
					KOS_ERROR
					
				}
			#endif
			
			#if KOS_USES_BCM
				printf("Initializing the BCM host ...\n");
				bcm_host_init();
				
				#if KOS_USES_OPENGLES
					bcm_ogles_init(__this);
				#endif
			#endif
		}
		
		printf("Initializing joysticks ...\n");
		
		#if KOS_USES_SDL2
			if (SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC) < 0) {
				printf("ERROR SDL2 could not initialize joysticks (%s)\n", SDL_GetError());
				KOS_ERROR
				
			}
		#endif
		
		kos_open_joysticks();
		
		#ifdef __HAS_X11
			printf("INFO Getting X11 display size ...\n");
			Display* display = XOpenDisplay(NULL);
			
			if (!display) {
				printf("WARNING Could not open X11 display\n");
				
			} else {
				Screen* screen = XScreenOfDisplay(display, 0);
				
				if (!screen) {
					printf("WARNING Could not get X11 default screen of display\n");
					
				} else {
					__this->width  = WidthOfScreen (screen);
					__this->height = HeightOfScreen(screen);
					
				}
				
				XCloseDisplay(display);
				
			}
		#endif
		
		if (!__this->text_only) {
			#if KOS_USES_SDL2
				SDL_GL_SetAttribute(SDL_GL_RED_SIZE,      5);
				SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,    6);
				SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,     5);
				
				SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,   16);
				SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,  1);
				
				//~ SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
				//~ SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);
				
				__this->window = SDL_CreateWindow("AQUA", \
					SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, \
					__this->width, __this->height, \
					SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
				
				printf("INFO Creating cursor ...\n");
				SDL_Cursor*   cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
				SDL_SetCursor(cursor);
				
				if (__this->window == NULL) {
					printf("ERROR SDL2 window could not be created (%s)\n", SDL_GetError());
					KOS_ERROR
					
				}
				
				SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
			#endif
			
			printf("Initializing fonts ...\n");
			kos_init_fonts();
			
			#if KOS_USES_SDL2 && KOS_USES_OPENGL
				__this->context = SDL_GL_CreateContext(__this->window);
				
				if (__this->context == NULL) {
					printf("ERROR SDL2 GL context could not be created (%s)\n", SDL_GetError());
					KOS_ERROR
					
				}
			#endif
			
			#if KOS_USES_OPENGL
				printf("OpenGL info\n");
				printf("\tVendor:                   %s\n", glGetString(GL_VENDOR));
				printf("\tRenderer:                 %s\n", glGetString(GL_RENDERER));
				printf("\tVersion:                  %s\n", glGetString(GL_VERSION));
				printf("\tShading language version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
				//~ printf("\tExtensions:               %s\n", glGetString(GL_EXTENSIONS));
				
				#if KOS_USES_OPENGL_DESKTOP
					KOS_BEST_GL_VERSION
					printf("Using OpenGL version %d.%d\n", kos_best_gl_version_major, kos_best_gl_version_minor);
				#elif KOS_USES_OPENGLES
					const char* version_string = (const char*) glGetString(GL_VERSION);
			
					if      (strstr(version_string, "OpenGL ES 3.") && gl3_stub_init()) printf("Using OpenGL ES version 3.X\n");
					else if (strstr(version_string, "OpenGL ES 2."))                    printf("Using OpenGL ES version 2.X\n");
					else                                                                printf("ERROR Unsupported OpenGL ES version (%s)\n", version_string);
			
					KOS_BEST_GL_VERSION
				#endif
			#endif
			
			#if KOS_USES_OPENGLES && KOS_USES_JNI
				__this->context = eglGetCurrentContext();
				
				extern bool               default_assets;
				bool old_default_assets = default_assets;
			
				int error = 0;
				default_assets = true;
				
				error += load_asset("vert.glsl",   &__this->vertex_shader);
				error += load_asset("frag.glsl", &__this->fragment_shader);
			
				if (error > 1) {
					printf("WARNING Failed to load shader assets, incrementing __this->warning_count ...\n");
					__this->warning_count++;
					
				}
			
				default_assets         = old_default_assets;
				__this->shader_program = create_program(__this->vertex_shader, __this->fragment_shader);
			
				if (!__this->shader_program) {
					printf("WARNING Failed to create shader_program, incrementing __this->warning_count ...\n");
					__this->warning_count++;
					
				}
				
				gl_use_shader_program(&__this->shader_program);
			#endif
			
			#if KOS_USES_SDL2
				SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, kos_best_gl_version_major);
				SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, kos_best_gl_version_minor);
			#endif
			
			#if KOS_USES_OPENGL
				#if !KOS_USES_JNI
					glEnable(GL_ALPHA);
					glEnable(GL_TEXTURE_2D);
				#endif
				
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glEnable(GL_DEPTH_TEST);
				
				#if !KOS_USES_SHADER_PIPELINE
					glEnable(GL_MULTISAMPLE);
					glEnable(GL_ALPHA_TEST);
					glAlphaFunc(GL_GREATER, 0.0f);
					
					glHint(GL_POINT_SMOOTH,   GL_NICEST);
					glHint(GL_LINE_SMOOTH,    GL_NICEST);
					//~ glHint(GL_POLYGON_SMOOTH, GL_NICEST);
 
					glEnable(GL_POINT_SMOOTH);
					glEnable(GL_LINE_SMOOTH);
					//~ glEnable(GL_POLYGON_SMOOTH);
				#endif
			
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
			
				glViewport(0, 0, __this->width, __this->height);
				
				#if KOS_3D_VISUALIZATION
					float fov   = tan(65.0f / 4);
					float ratio = 1.0f;
					
					float near = 0.1f;
					float far  = 500.0f;
					
					float center_x = 0.0f;
					float center_y = 0.0f;
					
					glFrustum( \
						near * (-fov * ratio + center_x), \
						near * ( fov * ratio + center_x), \
						near * (-fov         + center_y), \
						near * ( fov         + center_y), \
						near, far);
					
					glTranslatef(0.0f, 0.0f, -10.0f);
				#else
					glEnable   (GL_CULL_FACE);
					glCullFace (GL_BACK);
					glFrontFace(GL_CCW);
					
					glOrtho(-1.0f, 1.0f, -1.0f, 1.0f, -100.0f, 500.0f);
					glTranslatef(0.0f, 0.0f, -100.0f);
				#endif

				glMatrixMode(GL_MODELVIEW);
				glLoadIdentity();
			#endif
			
			#if KOS_USES_SDL2
				#if defined(KOS_ENABLE_VSYNC) && KOS_ENABLE_VSYNC
					if (SDL_GL_SetSwapInterval(-1) < 0) { // enable adaptive (-1) VSync
						printf("WARNING Failed to enable adaptive VSync (this may cause problems down the line)\n");
						__this->warning_count++;
						
					}
				#else
					SDL_GL_SetSwapInterval(0);
				#endif
			#endif
			
			printf("Setting up predefined textures ...\n");
			
			if (kos_setup_predefined_textures(__this)) {
				printf("WARNING Failed to setup predefined textures\n");
				__this->warning_count++;
				
			}
			
			#if KOS_USES_OPENGL
				GLint                                   default_fbo;
				glGetIntegerv(GL_FRAMEBUFFER_BINDING,  &default_fbo);
				printf("INFO Default OpenGL FBO: %d\n", default_fbo);
			#endif
		}
		
		printf("Finished KOS initialization with %d errors\n", __this->warning_count);
		return 0;
		
	}
	
	#include "functions/video.h"
	
#endif
