#include "shaders.h"

#define LOG_LENGTH 512


Shader::Shader(const char* vertexShaderPath, const char* fragmentShaderPath, int &success) {
	success = true;

	// read shader source code into strings
	std::string vertexShaderSourceStr, fragmentShaderSourceStr;

	if (
		!readFileToString(vertexShaderPath, vertexShaderSourceStr) |
		!readFileToString(fragmentShaderPath, fragmentShaderSourceStr)
		) {
		success = false;
		return;
	}

	// convert strings to const char*
	const char* vertexShaderSource = vertexShaderSourceStr.c_str();
	const char* fragmentShaderSource = fragmentShaderSourceStr.c_str();

	// create shader instances
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	// attach source code to shader instances
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);

	// compile shaders
	glCompileShader(vertexShader);
	glCompileShader(fragmentShader);

	// check for compile errors
	int vertexStatus, fragmentStatus;
	char log[LOG_LENGTH];

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexStatus);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragmentStatus);

	// log errors to user
	if (!vertexStatus) {
		glGetShaderInfoLog(vertexShader, LOG_LENGTH, NULL, log);
		std::cout << "Vertex shader compile failed:\n" << log << std::endl;
		success = false;
	}
	if (!fragmentStatus) {
		glGetShaderInfoLog(fragmentShader, LOG_LENGTH, NULL, log);
		std::cout << "Fragment shader compile failed:\n" << log << std::endl;
		success = false;
	}

	if (!success) return;


	// link shader program
	shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	int linkStatus;

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkStatus);

	if (!linkStatus) {
		glGetProgramInfoLog(shaderProgram, LOG_LENGTH, NULL, log);
		std::cout << "Shader program linking failed:\n" << log << std::endl;
		success = false;
		return;
	}

	// clean up before exiting
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

}


void Shader::use() {
	glUseProgram(shaderProgram);
}


void Shader::close() {
	glDeleteProgram(shaderProgram);
}


void Shader::setFloat(const char* name, float value) {
	glUniform1f(glGetUniformLocation(shaderProgram, name), value);
}

void Shader::vec2(const char* name, float value[2]) {
	glUniform2fv(glGetUniformLocation(shaderProgram, name), 1, value);
}

void Shader::vec3(const char* name, float value[3]) {
	glUniform3fv(glGetUniformLocation(shaderProgram, name), 1, value);
}

void Shader::vec4(const char* name, float value[4]) {
	glUniform4fv(glGetUniformLocation(shaderProgram, name), 1, value);
}


void Shader::setInt(const char* name, int value) {
	glUniform1i(glGetUniformLocation(shaderProgram, name), value);
}

void Shader::ivec2(const char* name, int value[2]) {
	glUniform2iv(glGetUniformLocation(shaderProgram, name), 1, value);
}

void Shader::ivec3(const char* name, int value[3]) {
	glUniform3iv(glGetUniformLocation(shaderProgram, name), 1, value);
}

void Shader::ivec4(const char* name, int value[4]) {
	glUniform4iv(glGetUniformLocation(shaderProgram, name), 1, value);
}


void Shader::setUint(const char* name, unsigned int value) {
	glUniform1ui(glGetUniformLocation(shaderProgram, name), value);
}

void Shader::uvec2(const char* name, unsigned int value[2]) {
	glUniform2uiv(glGetUniformLocation(shaderProgram, name), 1, value);
}

void Shader::uvec3(const char* name, unsigned int value[3]) {
	glUniform3uiv(glGetUniformLocation(shaderProgram, name), 1, value);
}

void Shader::uvec4(const char* name, unsigned int value[4]) {
	glUniform4uiv(glGetUniformLocation(shaderProgram, name), 1, value);
}

void Shader::mat2(const char* name, float value[4]) { // requires matrix in column major
	glUniformMatrix2fv(glGetUniformLocation(shaderProgram, name), 1, GL_FALSE, value);
}

void Shader::mat3(const char* name, float value[9]) {
	glUniformMatrix3fv(glGetUniformLocation(shaderProgram, name), 1, GL_FALSE, value);
}

void Shader::mat4(const char* name, float value[16]) {
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name), 1, GL_FALSE, value);
}