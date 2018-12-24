
#ifndef __AQUA__STD_STRUCTS_H
	#define __AQUA__STD_STRUCTS_H
	
	#include "macros.h"
	
	// internal, do not touch
	
	typedef float GLfloat;
	typedef unsigned char GLubyte;
	
	typedef struct {
		GLfloat x;
		GLfloat y;
		GLfloat z;
		
	} Vertex3D;
	
	typedef struct {
		GLfloat x;
		GLfloat y;
		
	} Coord2D;
	
	typedef struct {
		GLfloat red;
		GLfloat green;
		GLfloat blue;
		GLfloat alpha;
		
	} Color3D;
	
	// structs and types stuff
	
	typedef unsigned long long texture_t;
	typedef unsigned long long font_t;
	
	#define COMPAT_SURFACE_VERTEX_COUNT 8
	
	typedef struct {
		unsigned long long red;
		unsigned long long green;
		unsigned long long blue;
		
		signed long long width;
		signed long long height;
		
		signed long long x;
		signed long long y;
		
		signed long long layer;
		unsigned long long alpha;
		
		uint32_t has_texture;
		uint32_t resize_count;
		texture_t texture;
		
		unsigned long long scroll_texture;
		
		signed long long scroll_texture_x;
		signed long long scroll_texture_y;
		
		signed long long scroll_texture_width;
		signed long long scroll_texture_height;
		
		// internal, do not touch
		
		Vertex3D vertices     [COMPAT_SURFACE_VERTEX_COUNT];
		Color3D colours       [COMPAT_SURFACE_VERTEX_COUNT];
		Coord2D texture_coords[COMPAT_SURFACE_VERTEX_COUNT];
		GLubyte faces         [COMPAT_SURFACE_VERTEX_COUNT];
		
	} surface_t;
	
	typedef struct {
		unsigned long long width;
		unsigned long long height;

		unsigned long long x;
		unsigned long long y;

		unsigned long long red;
		unsigned long long green;
		unsigned long long blue;

	} rectangle_t;
	
	typedef struct {
		unsigned long long quit;
		
	} event_t;
	
	typedef struct {
		// event list, try to use this part the least possible (it's a pretty slow way of doing things)
		
		unsigned long long event_count;
		event_t events[256];
		
		// general events
		
		unsigned long long quit;
		
		unsigned long long pointer_click_type;
		
		unsigned long long pointer_x;
		unsigned long long pointer_y;
		
		unsigned long long resize;
		
	} event_list_t;

	typedef struct {
		char* address;
		
	} request_response_t;

	typedef struct {
		unsigned char header[64];
		unsigned long long data_position;
		
		unsigned long long width;
		unsigned long long height;
		
		unsigned long long image_size;
		unsigned long long* data;
		
		unsigned long long bpp;
		
	} bitmap_image_t;
	
	// internet stuff
	
	typedef unsigned long long* ip_address_t;
	
	typedef struct {
		unsigned long long error;		
		unsigned long long type; // either SOCKET_SERVER, SOCKET_CLIENT or SOCKET_MIXED (default)
		
		unsigned long long port;
		void* __internal_pointer; // this is handled by the KOS; the programmer should not use this
		
	} socket_t;
	
#endif
