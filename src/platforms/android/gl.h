//
// Created by obiwac on 15.07.18.
//

#ifndef ANDROID_GL_H
#define ANDROID_GL_H

#include <EGL/egl.h>

bool check_gl_error(const char* function_name) {
	GLint error = glGetError();

	if (error != GL_NO_ERROR) {
		const char* error_string;

		#define GL_TABLE_TOO_LARGE 0x8031
		#define GL_STACK_OVERFLOW  0x0503
		#define GL_STACK_UNDERFLOW 0x0504

		switch (error) {
			case GL_INVALID_ENUM:                  error_string = "GL_INVALID_ENUM";                  break;
			case GL_INVALID_VALUE:                 error_string = "GL_INVALID_VALUE";                 break;
			case GL_INVALID_OPERATION:             error_string = "GL_INVALID_OPERATION";             break;

			case GL_STACK_OVERFLOW:                error_string = "GL_STACK_OVERFLOW";                break;
			case GL_STACK_UNDERFLOW:               error_string = "GL_STACK_UNDERFLOW";               break;

			case GL_OUT_OF_MEMORY:                 error_string = "GL_OUT_OF_MEMORY";                 break;
			case GL_INVALID_FRAMEBUFFER_OPERATION: error_string = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
			case EGL_CONTEXT_LOST:                 error_string = "EGL_CONTEXT_LOST";                 break;
			case GL_TABLE_TOO_LARGE:               error_string = "GL_TABLE_TOO_LARGE";               break;

			default:                               error_string = "Unknown error";                    break;

		}

		ALOGE("WARNING GL error (%s) after `%s()` (error = 0x%04x)\n", error_string, function_name, error);

		return true;

	}

	return false;

}

GLuint create_shader(GLenum shader_type, const char* code) {
	GLuint shader = glCreateShader(shader_type);

	if (!shader) {
		check_gl_error("glCreateShader");
		return 0;

	}

	glShaderSource(shader, 1, &code, NULL);
	GLint compiled = GL_FALSE;

	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

	if (!compiled) {
		GLint info_log_length = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_length);

		if (info_log_length > 0) {
			GLchar* info_log = (GLchar*) malloc((size_t) info_log_length);

			if (info_log) {
				glGetShaderInfoLog(shader, info_log_length, NULL, info_log);
				ALOGE("ERROR Could not compile %s shader\n%s\n", shader_type == GL_VERTEX_SHADER ? "vertex" : "fragment", info_log);
				free(info_log);

			}

		}

		glDeleteShader(shader);
		return 0;

	}

	return shader;

}

GLuint create_program(const char* vertex_code, const char* fragment_code) {
	GLuint vertex_shader = 0;
	GLuint fragment_shader = 0;

	GLuint program = 0;
	GLint linked = GL_FALSE;

	vertex_shader = create_shader(GL_VERTEX_SHADER, vertex_code);

	if (vertex_shader) {
		fragment_shader = create_shader(GL_FRAGMENT_SHADER, fragment_code);

		if (fragment_shader) {
			program = glCreateProgram();

			if (!program) {
				check_gl_error("glCreateProgram");

			} else {
				glAttachShader(program, vertex_shader);
				glAttachShader(program, fragment_shader);

				glLinkProgram(program);
				glGetProgramiv(program, GL_LINK_STATUS, &linked);

				if (!linked) {
					ALOGE("ERROR Could not link program");

					GLint info_log_length = 0;
					glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_length);

					if (info_log_length) {
						GLchar* info_log = (GLchar*) malloc((size_t) info_log_length);

						if (info_log) {
							glGetProgramInfoLog(program, info_log_length, NULL, info_log);
							ALOGE("Could not link program:\n%s\n", info_log);
							free(info_log);

						}

					}

					glDeleteProgram(program);
					program = 0;

				}

			}

		}

	}

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	return program;

}

GLfloat projection_matrix[16];

void ortho(float left, float right, float top, float bottom, float near, float far) {
	projection_matrix[0]  =  2.0f / (right - left);
	projection_matrix[5]  =  2.0f / (top - bottom);
	projection_matrix[10] = -2.0f / (far - near);
	projection_matrix[15] =  1.0f;

	projection_matrix[12] = -((right + left) / (right - left));
	projection_matrix[13] = -((top + bottom) / (top - bottom));
	projection_matrix[14] = -((far + near)   / (far - near));

	projection_matrix[1]  = 0.0f;
	projection_matrix[2]  = 0.0f;
	projection_matrix[3]  = 0.0f;
	projection_matrix[4]  = 0.0f;
	projection_matrix[6]  = 0.0f;
	projection_matrix[7]  = 0.0f;
	projection_matrix[8]  = 0.0f;
	projection_matrix[9]  = 0.0f;
	projection_matrix[11] = 0.0f;

}

#endif
