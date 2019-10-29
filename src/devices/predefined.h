
#ifndef __AQUA__KOS_DEVICES_PREDEFINED_H
	#define __AQUA__KOS_DEVICES_PREDEFINED_H
	
	void kos_video_clear(unsigned long long zvm, unsigned long long r, unsigned long long g, unsigned long long b, unsigned long long a);
	
	static unsigned char predefined_textures_live = 0;
	static surface_t predefined_texture_surface_dummy;
	
	// predefined textures
	
	static texture_t predefined_background_texture       = 0;
	static texture_t predefined_frost_background_texture = 0;
	static texture_t predefined_white_texture            = 0;
	
	static unsigned long long predefined_background_texture_dimensions      [2];
	static unsigned long long predefined_frost_background_texture_dimensions[2];
	static unsigned long long predefined_white_texture_dimensions           [2];
	
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
			
			predefined_background_texture_dimensions      [0] = kos_video_width(0);
			predefined_background_texture_dimensions      [1] = kos_video_height(0);
			
			predefined_frost_background_texture_dimensions[0] = kos_video_width(0);
			predefined_frost_background_texture_dimensions[1] = kos_video_height(0);
			
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
			unsigned long long* result;
			char* data;
			unsigned long long bytes;
			
			fs_read((unsigned long long) "config/wallpaper/wallpaper.bmp", (unsigned long long) &data, (unsigned long long) &bytes);
			unsigned long long command[] = {(unsigned long long) data, bytes};
			bmp_device_handle(&result, (const char*) command);
			
			if (result) {
				predefined_background_texture               = result[0];
				predefined_background_texture_dimensions[0] = result[1];
				predefined_background_texture_dimensions[1] = result[2];
				
			}
			
			warning += !predefined_background_texture;
			
			fs_read((unsigned long long) "config/wallpaper/frost.bmp", (unsigned long long) &data, (unsigned long long) &bytes);
			unsigned long long command2[] = {(unsigned long long) data, bytes};
			bmp_device_handle(&result, (const char*) command2);
			
			if (result) {
				predefined_frost_background_texture               = result[0];
				predefined_frost_background_texture_dimensions[0] = result[1];
				predefined_frost_background_texture_dimensions[1] = result[2];
				
			}
			
			warning += !predefined_frost_background_texture;
			
			unsigned long long white_data[] = {0xFFFFFFFFFFFFFFFF};
			predefined_white_texture = texture_create((unsigned long long) white_data, 64, 1, 1);
			
			predefined_white_texture_dimensions[0] = 1;
			predefined_white_texture_dimensions[1] = 1;
			
			warning += !predefined_white_texture;
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
		if (predefined_white_texture            != -1) texture_remove(predefined_white_texture);
		
	}
	
	texture_t get_predefined_texture(const char* name) {
		if      (strcmp(name, "wallpaper") == 0) return predefined_background_texture;
		else if (strcmp(name, "frost")     == 0) return predefined_frost_background_texture;
		else if (strcmp(name, "white")     == 0) return predefined_white_texture;
		
		return 0;
		
	}
	
	static unsigned long long* get_predefined_texture_size(const char* name) {
		if      (strcmp(name, "wallpaper") == 0) return predefined_background_texture_dimensions;
		else if (strcmp(name, "frost")     == 0) return predefined_frost_background_texture_dimensions;
		else if (strcmp(name, "white")     == 0) return predefined_white_texture_dimensions;
		
		return (unsigned long long*) 0;
		
	}
	
	void update_predefined_textures(void) {
		if (predefined_textures_live) { // normal background
			GLuint default_shader = 0;
			
			framebuffer_bind(predefined_background_texture_framebuffer, 0, 0, kos_video_width(0), kos_video_height(0));
			gl_use_shader_program((GLuint*) &predefined_background_texture_shader);
			
			kos_video_clear(0, 0, 0, 0, 0);
			surface_draw((unsigned long long) &predefined_texture_surface_dummy);
			
			framebuffer_bind(0, 0, 0, kos_video_width(0), kos_video_height(0));
			gl_use_shader_program((GLuint*) &default_shader);
			
		} if (predefined_textures_live) { // frosted background
			GLuint default_shader = 0;
			
			framebuffer_bind(predefined_frost_background_texture_framebuffer, 0, 0, kos_video_width(0), kos_video_height(0));
			gl_use_shader_program((GLuint*) &predefined_frost_background_texture_shader);
			
			kos_video_clear(0, 0, 0, 0, 0);
			surface_draw((unsigned long long) &predefined_texture_surface_dummy);
			
			framebuffer_bind(0, 0, 0, kos_video_width(0), kos_video_height(0));
			gl_use_shader_program((GLuint*) &default_shader);
			
		}
		
	}
	
	static void predefined_device_handle(unsigned long long** result, const char* data) {
		unsigned long long* command = (unsigned long long*) data;
		kos_bda_implementation.temp_value = 0;
		*result = (unsigned long long*) &kos_bda_implementation.temp_value;
		
		if      (command[0] == 'w') kos_bda_implementation.temp_value = get_predefined_texture_size((const char*) command[1])[0];
		else if (command[0] == 'h') kos_bda_implementation.temp_value = get_predefined_texture_size((const char*) command[1])[1];
		
		else if (command[0] == 'g') kos_bda_implementation.temp_value = get_predefined_texture((const char*) command[1]);
		else KOS_DEVICE_COMMAND_WARNING("predefined")
		
	}
	
#endif
