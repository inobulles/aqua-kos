
#ifndef __AQUA__SDL2_SRC_GL_COMMON_SURFACE_H
	#define __AQUA__SDL2_SRC_GL_COMMON_SURFACE_H
	
	//static unsigned char shader_has_set_locations = 0;
	
	//static GLint shader_time_location;
	//static GLint shader_sampler_location;
	//static GLint shader_has_texture_location;
	
	#include "../gl_versions/surface/gl_1.h"
	#include "../gl_versions/surface/gl_2.h"
	#include "../gl_versions/surface/gl_3.h"
	#include "../gl_versions/surface/gl_4.h"
	#include "../gl_versions/surface/gl_5.h"
	
	//static unsigned long long resize_count;
	
	static inline void surface_set_layer_float(unsigned long long ____this, float layer) {
		surface_t* __this = (surface_t*) ____this;
		__this->layer = (signed long long) layer;
		
		int i;
		for (i = 0; i < SURFACE_VERTEX_COUNT; i++) {
			__this->vertices[i].z = 1.0f - (GLfloat) layer / 512.0f - 0.1f; /// FIXME
			
		}
		
	}
	
	void surface_set_layer(unsigned long long ____this, signed long long layer) {
		surface_t* __this = (surface_t*) ____this;
		surface_set_layer_float((unsigned long long) __this, (float) layer);
		
	}
	
	static inline void surface_update_vertices(unsigned long long ____this) {
		surface_t* __this = (surface_t*) ____this;
		surface_set_layer((unsigned long long) __this, __this->layer);
		
		float width  = (float) __this->width  / _UI64_MAX_MARGIN;
		float height = (float) __this->height / _UI64_MAX_MARGIN;
		
		float x      = (float) __this->x      / _UI64_MAX_MARGIN;
		float y      = (float) __this->y      / _UI64_MAX_MARGIN;
		
		int i;
		for (i = 0; i < 4; i++) {
			__this->vertices[i].x =           (GLfloat) (width  * vertex_matrix[i * 3]     + x);
			__this->vertices[i].y =           (GLfloat) (height * vertex_matrix[i * 3 + 1] + y);
			
			if (!__this->scroll_texture) {
				__this->texture_coords[i].x = (GLfloat) texture_coords[i * 2];
				__this->texture_coords[i].y = (GLfloat) texture_coords[i * 2 + 1];
				
			}
			
		}
		
	}
	
	void surface_scroll(unsigned long long ____this, signed long long _x, signed long long _y, unsigned long long _width, unsigned long long _height) {
		surface_t* __this = (surface_t*) ____this;
		__this->scroll_texture = 1;
		
		float x      = ((float) _x      / _SI64_MAX_MARGIN / 4) + 0.5f;
		float y      = ((float) _y      / _SI64_MAX_MARGIN / 4) + 0.5f;
		
		float width  =  (float) _width  / _UI64_MAX_MARGIN / 2;
		float height =  (float) _height / _UI64_MAX_MARGIN / 2;
		
		y = -y - height - 1.0f;
		
		int i;
		for (i = 0; i < 4; i++) {
			__this->texture_coords[i].x = (GLfloat) (texture_coords[i * 2]     * width  + x);
			__this->texture_coords[i].y = (GLfloat) (texture_coords[i * 2 + 1] * height + y);
			
		}
		
	}
	
	static void surface_update_colours(unsigned long long ____this) {
		surface_t* __this = (surface_t*) ____this;
		
		float red   = (float) __this->red   / _UI64_MAX;
		float green = (float) __this->green / _UI64_MAX;
		float blue  = (float) __this->blue  / _UI64_MAX;
		float alpha = (float) __this->alpha / _UI64_MAX;
		
		int i;
		for (i = 0; i < SURFACE_VERTEX_COUNT; i++) {
			__this->colours[i].alpha = alpha;
			
			__this->colours[i].red   = red;
			__this->colours[i].green = green;
			__this->colours[i].blue  = blue;
			
		}
		
	}

	static void surface_update(unsigned long long ____this) {
		surface_t* __this = (surface_t*) ____this;
		
		surface_update_vertices((unsigned long long) __this);
		surface_update_colours ((unsigned long long) __this);
		
	}

	static void surface_faces(unsigned long long ____this) {
		surface_t* __this = (surface_t*) ____this;
		
		__this->faces[0] = 0;
		__this->faces[1] = 1;
		__this->faces[2] = 2;
		
		__this->faces[3] = 0; // 3;
		__this->faces[4] = 2; // 4;
		__this->faces[5] = 3; // 5;
		
	}

	void surface_new(unsigned long long ____this, signed long long x, signed long long y, unsigned long long width, unsigned long long height) {
		surface_t* __this = (surface_t*) ____this;
		
		__this->red    = _UI64_MAX;
		__this->green  = _UI64_MAX;
		__this->blue   = _UI64_MAX;
		
		__this->x      = x;
		__this->y      = y;
		
		__this->width  = width;
		__this->height = height;
		
		__this->layer  = 0;
		__this->alpha  = _UI64_MAX;
		
		__this->scroll_texture = 0;
		__this->has_texture    = 0;
		
		surface_update((unsigned long long) __this);
		surface_faces ((unsigned long long) __this);
		
	}
	
	void surface_free(unsigned long long ____this) {
		//~ printf("FREED SURFACE %p\n", __this);
		
	}
	
	static float surface_layer_offset = 0.0f;
	
	int surface_draw(unsigned long long ____this) {
		surface_t* __this = (surface_t*) ____this;
		
		surface_set_layer_float((unsigned long long) __this, (float) __this->layer + surface_layer_offset);
		surface_layer_offset += 0.01f;
		
		if (surface_layer_offset >= 0.5f) {
			surface_layer_offset  = 0.0f;
			
		}
		
		switch (kos_best_gl_version_major) {
			case 1: return 1;
			case 2: return gl2_surface_draw(__this);
			case 3: return 1;
			case 4: return 1;
			case 5: return 1;
			
			default: {
				KOS_WARN_NO_GL_VERSION
				return 1;
				
			}
			
		}
		
	}
	
	void surface_set_texture(unsigned long long ____this, texture_t texture) {
		surface_t* __this = (surface_t*) ____this;
		
		__this->has_texture = 1;
		__this->texture = texture;
		
	}
	
	void surface_set_alpha(unsigned long long ____this, unsigned long long alpha) {
		surface_t* __this = (surface_t*) ____this;
		
		__this->alpha         = alpha;
		GLfloat float_alpha = (GLfloat) __this->alpha / _UI64_MAX;
		
		int i;
		for (i = 0; i < SURFACE_VERTEX_COUNT; i++) {
			__this->colours[i].alpha = float_alpha;
			
		}
		
	}
	
	void surface_set_colour(unsigned long long ____this, unsigned long long red, unsigned long long green, unsigned long long blue) {
		surface_t* __this = (surface_t*) ____this;
		
		__this->red   = red;
		__this->green = green;
		__this->blue  = blue;
		
		surface_update_colours((unsigned long long) __this);
		
	}
	
	void surface_set_x(unsigned long long ____this, signed long long x) {
		surface_t* __this = (surface_t*) ____this;
		
		__this->x = x;
		surface_update_vertices((unsigned long long) __this);
		
	}
	
	void surface_set_y(unsigned long long ____this, signed long long y) {
		surface_t* __this = (surface_t*) ____this;
		
		__this->y = y;
		surface_update_vertices((unsigned long long) __this);
		
	}
	
	void surface_set_width(unsigned long long ____this, unsigned long long width) {
		surface_t* __this = (surface_t*) ____this;
		
		__this->width = width;
		surface_update_vertices((unsigned long long) __this);
		
	}
	
	void surface_set_height(unsigned long long ____this, unsigned long long height) {
		surface_t* __this = (surface_t*) ____this;
		
		__this->height = height;
		surface_update_vertices((unsigned long long) __this);
		
	}
	
	// gradient functions
	
	#define KOS_SURF_FULL_COLOUR_FUNCTION_PARAMS unsigned long long ____this, unsigned long long red, unsigned long long green, unsigned long long blue, unsigned long long alpha
	#define SURFACE_GRADIENT_FUNCTIONS 1
	
	#define KOS_SURF_SET_COLOUR_FOR_VERTEX(index) { \
		__this->colours[index].red   = (float) red   / _UI64_MAX; \
		__this->colours[index].green = (float) green / _UI64_MAX; \
		__this->colours[index].blue  = (float) blue  / _UI64_MAX; \
		__this->colours[index].alpha = (float) alpha / _UI64_MAX; \
	}
	
	void surface_gradient_left(KOS_SURF_FULL_COLOUR_FUNCTION_PARAMS) {
		surface_t* __this = (surface_t*) ____this;
		
		KOS_SURF_SET_COLOUR_FOR_VERTEX(0)
		KOS_SURF_SET_COLOUR_FOR_VERTEX(1)
		
	}
	
	void surface_gradient_right(KOS_SURF_FULL_COLOUR_FUNCTION_PARAMS) {
		surface_t* __this = (surface_t*) ____this;
		
		KOS_SURF_SET_COLOUR_FOR_VERTEX(3)
		KOS_SURF_SET_COLOUR_FOR_VERTEX(2)
		
	}
	
	void surface_gradient_top(KOS_SURF_FULL_COLOUR_FUNCTION_PARAMS) {
		surface_t* __this = (surface_t*) ____this;
		
		KOS_SURF_SET_COLOUR_FOR_VERTEX(0)
		KOS_SURF_SET_COLOUR_FOR_VERTEX(3)
		
	}
	
	void surface_gradient_bottom(KOS_SURF_FULL_COLOUR_FUNCTION_PARAMS) {
		surface_t* __this = (surface_t*) ____this;
		
		KOS_SURF_SET_COLOUR_FOR_VERTEX(1)
		KOS_SURF_SET_COLOUR_FOR_VERTEX(2)
		
	}
	
#endif
