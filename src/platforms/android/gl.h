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

// thx http://www.manpagez.com/man/3/gl*

GLfloat model_view_matrix[16] = {0.999982, 0.000000, 0.000000, 0.000000, 0.000000, -1.000000, 0.000001, 0.000000, 0.000000, -0.000001, -1.000000, 0.000000, 0.000000, 32.000000, -0.000020, 1.000000};
GLfloat projection_matrix[16] = {0.622719, 0.000000, 0.000000, 0.000000, 0.000000, 0.830289, 0.000000, 0.000000, 0.000000, 0.000000, -1.000400, -1.000000, 0.000000, 0.000000, -0.200040, 0.000000};

#define MATRIX_BYTES (sizeof(GLfloat) * 4 * 4)

#define GL_MODELVIEW  0
#define GL_PROJECTION 1

int matrix_mode = GL_MODELVIEW;

void glLoadIdentity(void) {
	GLfloat* active_matrix = matrix_mode == GL_MODELVIEW ? (GLfloat*) model_view_matrix : (GLfloat*) projection_matrix;
	memset(active_matrix, 0, MATRIX_BYTES);
	
	active_matrix[0]  = 1.0f;
	active_matrix[5]  = 1.0f;
	active_matrix[10] = 1.0f;
	active_matrix[15] = 1.0f;
	
}

static inline void multiply_matrix_to(GLfloat* destination, GLfloat* a, GLfloat* b) {
	float temp[16];
	for (int i = 0; i < 4; i++) {
		float* ai = a    + i * 4;
		float* ti = temp + i * 4;
		
		for (int j = 0; j < 4; j++) {
			float tij = 0.0f;
			
			for (int k = 0; k < 4; k++) {
				tij += ai[k] * b[k * 4 + j];
				
			}
			
			ti[j] = tij;
			
		}
		
	}
	
	memcpy(destination, temp, MATRIX_BYTES);
	
}

void glTranslatef(float x, float y, float z) {
	GLfloat* active_matrix = matrix_mode == GL_MODELVIEW ? (GLfloat*) model_view_matrix : (GLfloat*) projection_matrix;
	
	active_matrix[12] += (active_matrix[0] * x + active_matrix[4] * y + active_matrix[8]  * z);
	active_matrix[13] += (active_matrix[1] * x + active_matrix[5] * y + active_matrix[9]  * z);
	active_matrix[14] += (active_matrix[2] * x + active_matrix[6] * y + active_matrix[10] * z);
	active_matrix[15] += (active_matrix[3] * x + active_matrix[7] * y + active_matrix[11] * z);
	
}

void glRotatef(float angle, float x, float y, float z) {
	double length = sqrt(x * x + y * y + z * z);
	
	x /= -length;
	y /=  length;
	z /=  length;
	
	float c = cos(angle);
	float s = sin(angle);
	
	GLfloat* active_matrix = matrix_mode == GL_MODELVIEW ? (GLfloat*) model_view_matrix : (GLfloat*) projection_matrix;
	GLfloat transformation_matrix[16] = {x * x * (1 - c) + c, x * y * (1 - c) - z * s, x * z * (1 - c) + y * s, 0, y * x * (1 - c) + z * s, y * y * (1 - c) + c, y * z * (1 - c) - x * s, 0, x * z * (1 - c) - y * s, y * z * (1 - c) + x * s, z * z * (1 - c) + c, 0, 0, 0, 0, 1};
	multiply_matrix_to(active_matrix, transformation_matrix, active_matrix);

}

void glMatrixMode(GLint mode) {
	matrix_mode = mode;
	
}

void glOrtho(float left, float right, float top, float bottom, float near, float far) {
	float tx = -(right + left) / (right - left);
	float ty = -(top + bottom) / (top - bottom);
	float tz = -(far + near) / (far - near);
	
	GLfloat* active_matrix = matrix_mode == GL_MODELVIEW ? (GLfloat*) model_view_matrix : (GLfloat*) projection_matrix;
	GLfloat transformation_matrix[16] = {2.0f / (right - left), 0, 0, tx, 0, 2.0f / (top - bottom), 0, ty, 0, 0, -2.0f / (far - near), tz, 0, 0, 0, 1};
	multiply_matrix_to(active_matrix, transformation_matrix, active_matrix);
	
}

void glFrustum(float left, float right, float top, float bottom, float near, float far) {
	/*left = (float) -0.160586;
	right = (float) 0.160586;
	
	GLfloat* active_matrix = matrix_mode == GL_MODELVIEW ? (GLfloat*) model_view_matrix : (GLfloat*) projection_matrix;
	
	GLfloat identity[] = {1.000000, 0.000000, 0.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 0.000000, 0.000000, 1.000000};
	memcpy(active_matrix, identity, MATRIX_BYTES);
	
	GLfloat transformation_matrix[16] = {(2.0f * near) / (right - left), 0, (right + left) / (right - left), 0, 0, (2.0f * near) / (top - bottom), (top + bottom) / (top - bottom), 0, 0, 0, -(far + near) / (far - near), -(2.0f * far * near) / (far - near), 0, 0, -1, 0};
	//multiply_matrix_to(active_matrix, transformation_matrix, active_matrix);
	memcpy(active_matrix, transformation_matrix, MATRIX_BYTES);
	
	ALOGA("ACTIVE %d: %f %f %f %f %f %f\n", matrix_mode == GL_MODELVIEW, left, right, top, bottom, near, far);
	for (int i = 0; i < 16; i++) {
		ALOGA("%f\n", active_matrix[i]); // (2 * 0.1) / (0.06022 - -0.06022) =
		
	}*/
	
	GLfloat _model_view_matrix[16] = {0.999982, 0.000000, 0.000000, 0.000000, 0.000000, -1.000000, 0.000001, 0.000000, 0.000000, -0.000001, -1.000000, 0.000000, 0.000000, 32.000000, -0.000020, 1.000000};
	GLfloat _projection_matrix[16] = {0.622719, 0.000000, 0.000000, 0.000000, 0.000000, 0.830289, 0.000000, 0.000000, 0.000000, 0.000000, -1.000400, -1.000000, 0.000000, 0.000000, -0.200040, 0.000000};
	
	GLfloat* active_matrix = matrix_mode == GL_MODELVIEW ? (GLfloat*) model_view_matrix : (GLfloat*) projection_matrix;
	memcpy(active_matrix, matrix_mode == GL_MODELVIEW ? (GLfloat*) _model_view_matrix : (GLfloat*) _projection_matrix, MATRIX_BYTES);
	
}

#endif
