
#ifndef __AQUA__KOS_FUNCTIONS_PREDEFINED_H
	#define __AQUA__KOS_FUNCTIONS_PREDEFINED_H
	
	static surface_t predefined_texture_surface_dummy;
	
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
	
	#define KOS_SUB2PEWDS 0//KOS_USES_OPENGL_DESKTOP
	
	static int kos_setup_predefined_textures(kos_t* __this) {
		int warning = 0;
		
		surface_new        ((unsigned long long) &predefined_texture_surface_dummy, -_UI64_MAX_MARGIN, -_UI64_MAX_MARGIN, _UI64_MAX_MARGIN << 1, _UI64_MAX_MARGIN << 1);
		surface_set_texture((unsigned long long) &predefined_texture_surface_dummy, 0);
		surface_scroll     ((unsigned long long) &predefined_texture_surface_dummy, -_UI64_MAX_MARGIN, _UI64_MAX_MARGIN, _UI64_MAX_MARGIN << 1, _UI64_MAX_MARGIN << 1);
		
		#if KOS_SUB2PEWDS
			predefined_textures_live = 1;
			
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
	
#endif
