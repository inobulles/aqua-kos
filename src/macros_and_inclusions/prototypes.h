
#ifndef __AQUA__SDL2_SRC_MACROS_AND_INCLUSIONS_PROTOTYPES_H
	#define __AQUA__SDL2_SRC_MACROS_AND_INCLUSIONS_PROTOTYPES_H

	#if !KOS_USES_JNI
		void glShaderSource(GLuint shader, GLsizei count, const GLchar** string, const GLint* length);
		void glCompileShader(GLuint shader);
		void glGetShaderiv(GLuint shader, GLenum pname, GLint* params);
		void glGetShaderInfoLog(GLuint shader, GLsizei maxLength, GLsizei* length, GLchar* infoLog);

		GLuint glCreateShader(GLenum shaderType);
		GLuint glCreateProgram(void);
		void glDeleteProgram(GLuint program);

		void glAttachShader(GLuint program, GLuint shader);
		void glLinkProgram(GLuint program);
		void glGetProgramiv(GLuint program, GLenum pname, GLint* params);
		void glGetProgramInfoLog(GLuint program, GLsizei maxLength, GLsizei* length, GLchar* infoLog);
		void glDeleteShader(GLuint shader);
		void glUseProgram(GLuint program);
	#endif
	
#endif
