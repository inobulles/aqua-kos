
#ifndef __AQUA__SDL2_SRC_FUNCTIONS_FONT_H
	#define __AQUA__SDL2_SRC_FUNCTIONS_FONT_H
	
	#include "../macros_and_inclusions.h"
	
	#if KOS_USES_SDL2
		#ifdef __USE_SDL_TTF
			#include <SDL2/SDL_ttf.h>
			#warning "WARNING Using the SDL_ttf library may cause problems on some platforms"
		#else
			#include <ft2build.h>
			#include FT_FREETYPE_H
			
			#include "SDL_ttf/SDL_ttf.c"
		#endif
	#endif
	
	#define __USE_SDL_TTF_PROVIDED 1
	
	#if __USE_SDL_TTF_PROVIDED && KOS_USES_SDL2
		typedef SDL_Surface* kos_font_surface_t;
	#else
		typedef struct {
			unsigned long long* pixels;
			
			unsigned long long  w;
			unsigned long long  h;
			
		} kos_font_surface_t;
	#endif
	
	typedef struct {
		unsigned char used;
		
		float size;
		char  path[MAX_PATH_LENGTH];
		char* text;
		
		kos_font_surface_t surface;
		
		#if KOS_USES_SDL2
			#ifdef __USE_SDL_TTF_PROVIDED
				TTF_Font* font;
			#else
				FT_Face font;
			#endif
		#endif
		
	} kos_font_t;
	
	#ifndef KOS_MAX_FONTS
		#define KOS_MAX_FONTS 4096
	#endif
	
	static kos_font_t kos_fonts[KOS_MAX_FONTS];
	
	#if __USE_SDL_TTF_PROVIDED && KOS_USES_SDL2
		static SDL_Color kos_font_colour;
	#elif KOS_USES_SDL2
		static FT_Library kos_freetype_library;
	#endif
	
	#ifndef KOS_CHECK_FONT
		#define KOS_CHECK_FONT(return_value) { \
			if (__this < 0 && __this >= KOS_MAX_FONTS && !kos_fonts[__this].used) { \
				printf("WARNING Font %lld does not exist\n", __this); \
				return (return_value); \
			} \
		}
	#endif
	
	static void kos_unuse_font(kos_font_t* __this) {
		__this->used = 0;
		__this->text = NULL;
		
		#if __USE_SDL_TTF_PROVIDED && KOS_USES_SDL2
			__this->font    = NULL;
			__this->surface = NULL;
		#endif
		
	}
	
	void kos_init_fonts(void) { /// TO... IMPLEMENT?
		#if __USE_SDL_TTF_PROVIDED && KOS_USES_SDL2
			kos_font_colour.r = 0xFF;
			kos_font_colour.g = 0xFF;
			kos_font_colour.b = 0xFF;
			kos_font_colour.a = 0xFF;
		#else
		#endif
		
		unsigned long long i;
		for (i = 0; i < KOS_MAX_FONTS; i++) {
			kos_unuse_font(&kos_fonts[i]);
			
		}
		
		#if __USE_SDL_TTF_PROVIDED && KOS_USES_SDL2
			if (TTF_Init() == -1) {
				printf("WARNING SDL2 TTF could not initialize (%s)\n", TTF_GetError());
				return;
				
			}
		#elif KOS_USES_SDL2
			if (FT_Init_FreeType(&kos_freetype_library)) {
				printf("WARNING FreeType could not initialize\n");
				return;
				
			}
		#endif
		
	}
	
	void kos_destroy_fonts(void) {
		#if __USE_SDL_TTF_PROVIDED && KOS_USES_SDL2
			TTF_Quit();
		#elif KOS_USES_SDL2
			FT_Done_FreeType(kos_freetype_library);
		#endif
		
	}
	
	#if !(__USE_SDL_TTF_PROVIDED) && KOS_USES_SDL2
		unsigned long long kos_freetype_new_font(const char* path, unsigned long long size, FT_Face* font) {
			unsigned long long font_loading_error = FT_New_Face(kos_freetype_library, path, 0, font);
			
			if (font_loading_error == FT_Err_Unknown_File_Format) printf("WARNING Font could not be loaded (unknown file format)\n");
			if (FT_Set_Char_Size(*font, 0, size << 6, 300, 300))  printf("WARNING Failed to set font size\n");
			
			return font_loading_error;
			
		}
	#endif
	
	unsigned long long video_width(void);
	
	font_t new_font(unsigned long long ___path, unsigned long long size) {
		const char* _path = (const char*) ___path;
		GET_PATH((char*) _path);
		
		#if KOS_USES_JNI
			jint error = CALLBACK_INT(java_new_font, (jint) (((float) size / _UI64_MAX) * (float) video_width()), callback_env->NewStringUTF(path));
			
			if (error < 0) {
				ALOGE("WARNING Java had a problem loading the font\n");
				
			}
		
			return (font_t) error;
			
		#else
			unsigned long long i;
			for (i = 0; i < KOS_MAX_FONTS; i++) {
				if (kos_fonts[i].used == 0) {
					kos_fonts[i].used =  1;
					
					memcpy(kos_fonts[i].path, path, MAX_PATH_LENGTH * sizeof(char));
					
					kos_fonts[i].size = (float) size / _UI64_MAX;
					unsigned char font_loading_error = 0;
					unsigned long long _size = (unsigned long long) (kos_fonts[i].size * video_width());
					
					#if __USE_SDL_TTF_PROVIDED && KOS_USES_SDL2
						kos_fonts[i].font  = TTF_OpenFont(kos_fonts[i].path, _size);
						font_loading_error = !kos_fonts[i].font;
					#elif KOS_USES_SDL2
						font_loading_error = kos_freetype_new_font(kos_fonts[i].path, _size, &kos_fonts[i].font);
					#endif
					
					if (font_loading_error) {
						printf("WARNING Font could not be loaded (possibly an incorrect path? `%s`)\n", path);
						kos_fonts[i].used = 0;
						
						return (font_t) -1;
						
					}
					
					return i;
					
				}
				
			}
			
			printf("WARNING You have surpassed the maximum font count (KOS_MAX_FONTS = %d)\n", KOS_MAX_FONTS);
			return (font_t) -1;
		#endif
		
	}
	
	void update_all_font_sizes(void) {
		unsigned long long i;
		for (i = 0; i < KOS_MAX_FONTS; i++) {
			if (kos_fonts[i].used) {
				unsigned char font_loading_error = 0;
				unsigned long long size = (unsigned long long) (kos_fonts[i].size * video_width());
				
				#if __USE_SDL_TTF_PROVIDED && KOS_USES_SDL2
					TTF_CloseFont(kos_fonts[i].font);
					kos_fonts[i].font = TTF_OpenFont(kos_fonts[i].path, size);
				#elif KOS_USES_SDL2
					FT_Done_Face(kos_fonts[i].font);
					kos_freetype_new_font(kos_fonts[i].path, size, &kos_fonts[i].font);
				#endif
				
			}
			
		}
		
	}
	
	static void kos_font_create_text(kos_font_t* __this, unsigned long long __text) {
		char* text = (char*) __text;
		
		if (
		#if __USE_SDL_TTF_PROVIDED && KOS_USES_SDL2
			!__this->surface ||
		#endif
			(__this->text == NULL || strcmp(text, __this->text) != 0)) {
			#if __USE_SDL_TTF_PROVIDED && KOS_USES_SDL2
				if (__this->surface) {
					SDL_FreeSurface(__this->surface);
					__this->surface = NULL;
					
				}
			#endif
			
			if      (__this->text) {
				free(__this->text);
				
			}
			
			__this->text = (char*) malloc(strlen(text) + 1);
			strcpy(__this->text,                 text);
			
			#if __USE_SDL_TTF_PROVIDED && KOS_USES_SDL2
				SDL_Surface* temp = TTF_RenderUTF8_Blended(__this->font, text, kos_font_colour);
				__this->surface = SDL_CreateRGBSurface(0, temp->w, temp->h, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
				
				SDL_BlitSurface(temp, NULL, __this->surface, NULL);
				SDL_FreeSurface(temp);
				
				if (!__this->surface) {
					printf("WARNING Could not create the font surface (SDL `%s`, TTF `%s`)\n", SDL_GetError(), TTF_GetError());
					__this->used = 0;
					
					return;
					
				}
				
				SDL_LockSurface(__this->surface);
				uint64_t* pixels = (uint64_t*) __this->surface->pixels;
				
				unsigned long long i;
				for (i = 0; i < (__this->surface->w * __this->surface->h) / 2; i++) {
					pixels[i] &= 0xFF000000FF000000;
					pixels[i] += 0x00FFFFFF00FFFFFF;
					
				}
			#elif KOS_USES_SDL2
				FT_Matrix matrix;
				
				matrix.xx = (FT_Fixed) (1 * 0x10000L);
				matrix.xy = (FT_Fixed) (0 * 0x10000L);
				matrix.yx = (FT_Fixed) (0 * 0x10000L);
				matrix.yy = (FT_Fixed) (1 * 0x10000L);
				
				FT_Vector position;
				
				position.x = 0;
				position.y = 0;
				
				FT_GlyphSlot slot = __this->font->glyph;
				
				__this->surface.w = 1000;
				__this->surface.h = 1000;
				__this->surface.pixels = (unsigned long long*) malloc(__this->surface.w * __this->surface.h * 4);
				
				unsigned long long i;
				for (i = 0; i < strlen(text) - 1; i++) {
					FT_Set_Transform(__this->font, &matrix, &position);
					
					if (FT_Load_Char(__this->font, text[i], FT_LOAD_RENDER)) {
						printf("WARNING Failed to load \"%d\" (or %c) character\n", text[i], text[i]);
						continue;
						
					}
					
					FT_Bitmap bitmap = slot->bitmap;
					
					unsigned long long ox = slot->bitmap_left;
					unsigned long long oy = slot->bitmap_top;
					
					unsigned long long xmax = ox + bitmap.width;
					unsigned long long ymax = oy + bitmap.rows;
					
					unsigned long long x;
					unsigned long long y;
					
					unsigned long long p;
					unsigned long long q;
					
					printf("%lld %lld\n", xmax, ymax);
					
					for (x = ox; x < xmax; x++, p++) {
						for (y = oy; y < ymax; y++, q++) {
							if (x < 0 || y < 0) continue;
							((char*) __this->surface.pixels)[q * bitmap.width + p] |= bitmap.buffer[q * bitmap.width + p];
							
						}
						
					}
					
					position.x += slot->advance.x;
					position.y += slot->advance.y;
					
				}
			#endif
			
		}
		
	}
	
	unsigned long long font_remove(font_t __this) {
		#if KOS_USES_JNI
			//CALLBACK_VOID(java_font_remove, font); // This was replaced by Lib.clear_fonts, called in InstanceActivity.dispose_all
		#else
			KOS_CHECK_FONT(-1)
			
			if (kos_fonts[__this].text != NULL) {
				free(kos_fonts[__this].text);
				
			}
			
			#if __USE_SDL_TTF_PROVIDED && KOS_USES_SDL2
				if (kos_fonts[__this].surface) {
					SDL_FreeSurface(kos_fonts[__this].surface);
					
				}
				
				TTF_CloseFont(kos_fonts[__this].font);
			#elif KOS_USES_SDL2
				FT_Done_Face(kos_fonts[__this].font);
			#endif
			
			kos_unuse_font(&kos_fonts[__this]);
		#endif
		
		return 0;
		
	}

	unsigned long long get_font_width(font_t __this, unsigned long long __text) {
		char* text = (char*) __text;
		
		#if KOS_USES_JNI
			return (unsigned long long) CALLBACK_INT(java_get_font_width, (jint) __this, callback_env->NewStringUTF((const char*) text));
		#else
			KOS_CHECK_FONT(-1)
			
			kos_font_t* font = &kos_fonts[__this];
			kos_font_create_text(font, (unsigned long long) text);
			
			#if __USE_SDL_TTF_PROVIDED && KOS_USES_SDL2
				return font->surface->w;
			#elif KOS_USES_SDL2
				return font->surface.w;
			#else
				return 100;
			#endif
		#endif
		
	}
	
	unsigned long long get_font_height(font_t __this, unsigned long long __text) {
		char* text = (char*) __text;
		
		#if KOS_USES_JNI
			return (unsigned long long) CALLBACK_INT(java_get_font_height, (jint) __this, callback_env->NewStringUTF((const char*) text));
		#else
			KOS_CHECK_FONT(-1)
			
			kos_font_t* font = &kos_fonts[__this];
			kos_font_create_text(font, (unsigned long long) text);
			
			#if __USE_SDL_TTF_PROVIDED && KOS_USES_SDL2
				return font->surface->h;
			#elif KOS_USES_SDL2
				return font->surface.h;
			#else
				return 100;
			#endif
		#endif
		
	}

	texture_t create_texture_from_font(font_t __this, unsigned long long __text) {
		char* text = (char*) __text;
		
		#if KOS_USES_JNI
			texture_t texture = 0;
			jint      error   = 1;
		
			unsigned long long width  = get_font_width (__this, (unsigned long long) text);
			unsigned long long height = get_font_height(__this, (unsigned long long) text);
		
			if (!(width <= 0 || height <= 0)) {
				error = CALLBACK_INT(java_create_texture_from_font, (jint) __this, callback_env->NewStringUTF((const char*) text), TEXTURE_WRAP_TYPE, SHARP_TEXTURES);
				
			}
		
			if (error < 0) ALOGE("WARNING Java had a problem loading the font\n");
			else           texture = (texture_t) error;
		
			return texture;
		#else
			KOS_CHECK_FONT(0)
			
			kos_font_t* font = &kos_fonts[__this];
			kos_font_create_text(font, (unsigned long long) text);
			
			#if __USE_SDL_TTF_PROVIDED && KOS_USES_SDL2
				return __texture_create(font->surface->pixels, 32, font->surface->w, font->surface->h, 0);
			#elif KOS_USES_SDL2
				return __texture_create(font->surface.pixels, 32, font->surface.w, font->surface.h, 0);
			#else
				return 0;
			#endif
		#endif
		
	}
	
#endif
