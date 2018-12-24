
#ifndef __AQUA__SDL2_SRC_KOS_GL_VERSIONS_GL_2_H
	#define __AQUA__SDL2_SRC_KOS_GL_VERSIONS_GL_2_H
	
	#include "../../macros_and_inclusions.h"
	
	static int gl2_create_shader(GLuint shader, char* code) {
		glShaderSource (shader, 1, (const GLchar**) &code, NULL);
		glCompileShader(shader);
		
		GLint error;
		int log_length;
		
		glGetShaderiv(shader, GL_COMPILE_STATUS, &error);
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
		
		if (log_length > 0) {
			char* error_message = (char*) malloc(log_length + 1);
			glGetShaderInfoLog(shader, log_length, NULL, (GLchar*) error_message);
			
			printf("ERROR Failed to compile shader (%s)\n", error_message);
			
			free(error_message);
			return 1;
			
		} else {
			return 0;
			
		}
		
	}
	
	int gl2_load_shaders(GLuint* program, char* vertex_code, char* fragment_code) {
		GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
		
		if (gl2_create_shader(vertex_shader, vertex_code)) return 1;
		if (gl2_create_shader(fragment_shader, fragment_code)) return 1;
		
		*program = glCreateProgram();
		
		glAttachShader(*program, vertex_shader);
		glAttachShader(*program, fragment_shader);
		
		glLinkProgram(*program);
		
		GLint error;
		int log_length;
		
		glGetProgramiv(*program, GL_LINK_STATUS, &error);
		glGetProgramiv(*program, GL_INFO_LOG_LENGTH, &log_length);
		
		if (log_length > 0) {
			char* error_message = (char*) malloc(log_length + 1);
			glGetProgramInfoLog(*program, log_length, NULL, error_message);
			
			printf("ERROR Failed to link program (%s)\n", error_message);
			
			free(error_message);
			return 1;
			
		}
		
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);
		
		return 0;
		
	}
	
	void gl2_use_shader_program(GLuint* program) {
		glUseProgram(*program);
		
	}
	
	void gl2_delete_shader_program(GLuint* program) {
		glDeleteProgram(*program);
		
	}
	
#endif
