
static unsigned char shader_has_set_locations;

static GLint shader_mvp_matrix_location = -1;
static GLint shader_time_location = -1;

static GLint shader_sampler_location = -1;
static GLint shader_has_texture_location = -1;

static GLint shader_width_location = -1;
static GLint shader_height_location = -1;

static int gl_create_shader(GLuint shader, char* code) {
	glShaderSource (shader, 1, (const GLchar**) &code, NULL);
	glCompileShader(shader);
	
	GLint compile_status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);
	
	if (compile_status == GL_FALSE) {
		int log_length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
		
		if (log_length > 0) {
			char* error_message = (char*) malloc(log_length);
			glGetShaderInfoLog(shader, log_length, NULL, error_message);
			
			printf("ERROR Failed to compile shader (%s)\n", error_message);
			free(error_message);
			
		} else {
			printf("ERROR Failed to compile shader for some unknown reason\n");
		}
		
		return 1;
	}
	
	return 0;
}

int gl_load_shaders(GLuint* program, char* vertex_code, char* fragment_code) {
	GLuint   vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	
	if (gl_create_shader(vertex_shader,     vertex_code)) return 1;
	if (gl_create_shader(fragment_shader, fragment_code)) return 1;
	
	*program = glCreateProgram();
	
	glAttachShader(*program, vertex_shader);
	glAttachShader(*program, fragment_shader);
	
	glBindAttribLocation(*program, 0, "vertex_position");
	glBindAttribLocation(*program, 1, "texture_coord");
	glBindAttribLocation(*program, 2, "vertex_colour");
	
	glLinkProgram(*program);
	
	GLint link_status;
	glGetProgramiv(*program, GL_LINK_STATUS, &link_status);
	
	if (link_status == GL_FALSE) {
		int log_length;
		glGetProgramiv(*program, GL_INFO_LOG_LENGTH, &log_length);
		
		if (log_length > 0) {
			char* error_message = (char*) malloc((unsigned) (log_length + 1));
			glGetProgramInfoLog(*program, log_length, NULL, error_message);
			
			printf("ERROR Failed to link program (%s)\n", error_message);
			free(error_message);
			
		} else {
			printf("ERROR Failed to link program for some unkown reason\n");
		}
		
		return 1;
	}
	
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	
	return 0;
}

unsigned long long gl_shader_get_uniform_location(GLuint* program, const char* string) {
	return glGetUniformLocation(*program, string);
}

void gl_shader_set_uniform_location(GLint uniform_location, unsigned long long data, unsigned long long type) {
	switch (type) {
		case 'f': glUniform1f(uniform_location, data); break;
		default: glUniform1i(uniform_location, (GLint) data); break;
	}
}

void gl_use_shader_program(GLuint* program) {
	shader_has_set_locations = 1;
	
	shader_mvp_matrix_location  = glGetUniformLocation(*program, "mvp_matrix");
	shader_time_location        = glGetUniformLocation(*program, "time");
	
	shader_sampler_location     = glGetUniformLocation(*program, "sampler_texture");
	shader_has_texture_location = glGetUniformLocation(*program, "has_texture");
	
	shader_width_location       = glGetUniformLocation(*program, "width");
	shader_height_location      = glGetUniformLocation(*program, "height");
	
	glUseProgram(*program);
	
	if (shader_time_location   >= 0) glUniform1i(shader_time_location,   (GLint) clock() / 100);
	if (shader_width_location  >= 0) glUniform1i(shader_width_location,  (GLint) kos_video_width (0));
	if (shader_height_location >= 0) glUniform1i(shader_height_location, (GLint) kos_video_height(0));
}

void gl_delete_shader_program(GLuint* program) {
	glDeleteProgram(*program);
}

static void shader_device_handle(unsigned long long** result, const char* data) {
	const unsigned long long* shader = (const unsigned long long*) data;
	
	if      (shader[0] == 'c') kos_bda_implementation.temp_value = gl_load_shaders((GLuint*) shader[1], (char*) shader[2], (char*) shader[3]);
	else if (shader[0] == 'r') gl_delete_shader_program((GLuint*) shader[1]);
	else if (shader[0] == 'u') gl_use_shader_program   ((GLuint*) shader[1]);
	
	else if (shader[0] == 'g') kos_bda_implementation.temp_value = gl_shader_get_uniform_location((GLuint*) shader[1], (const char*) shader[2]);
	else if (shader[0] == 's') gl_shader_set_uniform_location((GLint) shader[1], shader[2], shader[3]);
	
	*result = (unsigned long long*) &kos_bda_implementation.temp_value;
}
