
#ifndef __AQUA__SDL2_SRC_FUNCTIONS_FONT_H
	#define __AQUA__SDL2_SRC_FUNCTIONS_FONT_H
	
	#ifndef STB_TRUETYPE_IMPLEMENTATION
		#define STB_TRUETYPE_IMPLEMENTATION
		#include "../external/libstb/stb_truetype.h"
	#endif
	
	#include <locale.h>
	
	typedef struct {
		unsigned long long* pixels;
		unsigned long long w, h;
		unsigned long long x, y;
		
	} kos_font_surface_t;
	
	typedef struct {
		unsigned char used;
		
		float size;
		char* text;
		
		stbtt_fontinfo font;
		kos_font_surface_t surface;
		
	} kos_font_t;
	
	static unsigned long long kos_font_count = 0;
	static kos_font_t* kos_fonts = (kos_font_t*) 0;
	
	void kos_init_fonts(void) {
		kos_font_count = 1;
		kos_fonts = (kos_font_t*) malloc(kos_font_count * sizeof(kos_font_t));
		memset(&*kos_fonts, 0, sizeof(*kos_fonts));
		
	}
	
	static inline void font_remove_internal(kos_font_t* self) {
		if (self->text) {
			free(self->text);
			self->text = (char*) 0;
			
		} if (self->surface.pixels) {
			free(self->surface.pixels);
			self->surface.pixels = (unsigned long long*) 0;
			
		}
		
	}
	
	unsigned long long font_remove(unsigned long long __self) {
		kos_font_t* self = &kos_fonts[__self];
		
		if (self->used) {
			self->used = 0;
			font_remove_internal(self);
			return 0;
			
		}
		
		return 1;
		
	}
	
	void kos_destroy_fonts(void) {
		for (unsigned long long i = 0; i < kos_font_count; i++) font_remove(i);
		mfree(kos_fonts, kos_font_count * sizeof(kos_font_t));
		
	}
	
	unsigned long long new_font(unsigned long long data, unsigned long long bytes) {
		kos_fonts = (kos_font_t*) realloc(kos_fonts, (kos_font_count + 1) * sizeof(kos_font_t));
		kos_font_t* self = &kos_fonts[kos_font_count];
		memset(self, 0, sizeof(*self));
		
		self->used = 1;
		self->size = 0.05f;
		
		return stbtt_InitFont(&self->font, (const unsigned char*) data, stbtt_GetFontOffsetForIndex((const unsigned char*) data, 0)) ? kos_font_count++ : 0;
		
	} unsigned long long font_size(unsigned long long __self, unsigned long long size) {
		kos_fonts[__self].size = (float) size / _UI64_MAX;
		return 0;
		
	}
	
	unsigned long long video_width(void);
	static void kos_font_create_text(kos_font_t* self, unsigned long long __text) {
		STBTT_FLATNESS = 0.2f;
		char* text = (char*) __text;
		
		if (self->text == (char*) 0 || strcmp(text, self->text) != 0) {
			font_remove_internal(self);
			
			unsigned long long bytes = strlen(text) + 1;
			self->text = (char*) malloc(bytes);
			memcpy(self->text, text, bytes);
			
			int __ascent, __descent, __line_gap;
			float scale = stbtt_ScaleForPixelHeight(&self->font, (int) (self->size * video_width()));
			stbtt_GetFontVMetrics(&self->font, &__ascent, &__descent, &__line_gap);
			
			float ascent   = scale * __ascent;
			float descent  = scale * __descent;
			float line_gap = scale * __line_gap;
			
			float baseline = ascent;
			float line_spacing = ascent - descent + line_gap;
			
			unsigned long long bitmap_count = 0;
			kos_font_surface_t* bitmaps = (kos_font_surface_t*) malloc((bitmap_count + 1) * sizeof(kos_font_surface_t));
			
			float x = 2.0f, y = 2.0f;
			self->surface.w = self->surface.h = 0;
			
			for (unsigned long long i = 0; i < bytes - 1; i++) { /// TODO UTF-8, wrapped text
				int current = self->text[i];
				
				if (current == '\n' ) {
					y += line_spacing;
					if (bitmap_count) self->surface.w = fmax(self->surface.w, x + bitmaps[bitmap_count - 1].w);
					x = 2.0f;
					
				} else {
					int next = self->text[i + 1];
					
					float shiftx = x - (float) floor(x);
					float shifty = y - (float) floor(y);
					
					int advance_width, left_side_bearing;
					stbtt_GetCodepointHMetrics(&self->font, current, &advance_width, &left_side_bearing);
					
					int x0, y0, x1, y1;
					stbtt_GetCodepointBitmapBoxSubpixel(&self->font, current, scale, scale, shiftx, shifty, &x0, &y0, &x1, &y1);
					
					bitmaps = (kos_font_surface_t*) realloc(bitmaps, (bitmap_count + 1) * sizeof(kos_font_surface_t));
					
					int w, h;
					int offx, offy;
					bitmaps[bitmap_count].pixels = (unsigned long long*) stbtt_GetCodepointBitmapSubpixel(&self->font, scale, scale, shiftx, shifty, current, &w, &h, &offx, &offy);
					
					bitmaps[bitmap_count].w = w;
					bitmaps[bitmap_count].h = h;
					
					bitmaps[bitmap_count].x = offx + x;
					bitmaps[bitmap_count].y = y + baseline + y0;
					
					bitmap_count++;
					x += scale * advance_width;
					
					if (next) {
						x += scale * stbtt_GetCodepointKernAdvance(&self->font, current, next);
						
					}
					
				}
				
			}
			
			self->surface.w = fmax(self->surface.w, x + bitmaps[bitmap_count - 1].w);
			self->surface.h = y + line_spacing;
			
			bytes = self->surface.w * self->surface.h * 4;
			self->surface.pixels = (unsigned long long*) malloc(bytes);
			memset(self->surface.pixels, 0xFF, bytes);
			for (unsigned long long i = 0; i < self->surface.w * self->surface.h; i++) ((char*) self->surface.pixels)[i * 4 + 3] = 0x00;
			
			for (unsigned long long i = 0; i < bitmap_count; i++) {
				for (unsigned long long j = 0; j < bitmaps[i].w * bitmaps[i].h; j++) {
					if (bitmaps[i].w) {
						unsigned long long k = (bitmaps[i].x + j % bitmaps[i].w + (bitmaps[i].y + j / bitmaps[i].w) * self->surface.w) * 4 + 3;
						if (k < bytes) ((char*) self->surface.pixels)[k] = ((char*) bitmaps[i].pixels)[j];
						
					}
					
				}
				
				mfree(bitmaps[i].pixels, bitmaps[i].w * bitmaps[i].h);
				
			}
			
			mfree(bitmaps, (bitmap_count + 1) * sizeof(*bitmaps));
			
		}
		
	}
	
	unsigned long long get_font_width(unsigned long long self, unsigned long long text) {
		kos_font_create_text(&kos_fonts[self], text);
		return kos_fonts[self].surface.w;
		
	} unsigned long long get_font_height(unsigned long long self, unsigned long long text) {
		kos_font_create_text(&kos_fonts[self], text);
		return kos_fonts[self].surface.h;
		
	}

	texture_t create_texture_from_font(unsigned long long __self, unsigned long long text) {
		kos_font_t* self = &kos_fonts[__self];
		kos_font_create_text(self, text);
		return __texture_create(self->surface.pixels, 32, self->surface.w, self->surface.h, 0);
		
	}
	
	static void font_device_handle(unsigned long long** result, const char* data) {
		unsigned long long* command = (unsigned long long*) data;
		*result = (unsigned long long*) &kos_bda_implementation.temp_value;
		
		if      (command[0] == 'c') kos_bda_implementation.temp_value = new_font(command[1], command[2]);
		else if (command[0] == 'r') kos_bda_implementation.temp_value = font_remove(command[1]);
		
		else if (command[0] == 's') kos_bda_implementation.temp_value = font_size(command[1], command[2]);
		
		else if (command[0] == 'w') kos_bda_implementation.temp_value = get_font_width (command[1], command[2]);
		else if (command[0] == 'h') kos_bda_implementation.temp_value = get_font_height(command[1], command[2]);
		
		else if (command[0] == 't') kos_bda_implementation.temp_value = create_texture_from_font(command[1], command[2]);
		else KOS_DEVICE_COMMAND_WARNING("font")
		
	}
	
#endif
