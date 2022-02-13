//#include "shaders.h"
//
//#define LOG_LENGTH 512
//
//
//Shader::Shader(const char* vertexShaderPath, const char* fragmentShaderPath, int &success) {
//	success = true;
//
//	// read shader source code into strings
//	std::string vertexShaderSourceStr, fragmentShaderSourceStr;
//
//	if (
//		!readFileToString(vertexShaderPath, vertexShaderSourceStr) |
//		!readFileToString(fragmentShaderPath, fragmentShaderSourceStr)
//		) {
//		success = false;
//		return;
//	}
//
//	// convert strings to const char*
//	const char* vertexShaderSource = vertexShaderSourceStr.c_str();
//	const char* fragmentShaderSource = fragmentShaderSourceStr.c_str();
//
//	// create shader instances
//	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
//	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//
//	// attach source code to shader instances
//	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
//	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
//
//	// compile shaders
//	glCompileShader(vertexShader);
//	glCompileShader(fragmentShader);
//
//	// check for compile errors
//	int vertexStatus, fragmentStatus;
//	char log[LOG_LENGTH];
//
//	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexStatus);
//	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragmentStatus);
//
//	// log errors to user
//	if (!vertexStatus) {
//		glGetShaderInfoLog(vertexShader, LOG_LENGTH, NULL, log);
//		std::cout << "Vertex shader compile failed:\n" << log << std::endl;
//		success = false;
//	}
//	if (!fragmentStatus) {
//		glGetShaderInfoLog(fragmentShader, LOG_LENGTH, NULL, log);
//		std::cout << "Fragment shader compile failed:\n" << log << std::endl;
//		success = false;
//	}
//
//	if (!success) return;
//
//
//	// link shader program
//	shaderProgram = glCreateProgram();
//
//	glAttachShader(shaderProgram, vertexShader);
//	glAttachShader(shaderProgram, fragmentShader);
//	glLinkProgram(shaderProgram);
//
//	int linkStatus;
//
//	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkStatus);
//
//	if (!linkStatus) {
//		glGetProgramInfoLog(shaderProgram, LOG_LENGTH, NULL, log);
//		std::cout << "Shader program linking failed:\n" << log << std::endl;
//		success = false;
//		return;
//	}
//
//	// clean up before exiting
//	glDeleteShader(vertexShader);
//	glDeleteShader(fragmentShader);
//
//}
//
//
//void Shader::use() {
//	glUseProgram(shaderProgram);
//}
//
//
//void Shader::close() {
//	glDeleteProgram(shaderProgram);
//}
//
//
//void Shader::setFloat(const char* name, float value) {
//	glUniform1f(glGetUniformLocation(shaderProgram, name), value);
//}
//
//void Shader::vec2(const char* name, float value[2]) {
//	glUniform2fv(glGetUniformLocation(shaderProgram, name), 1, value);
//}
//
//void Shader::vec3(const char* name, float value[3]) {
//	glUniform3fv(glGetUniformLocation(shaderProgram, name), 1, value);
//}
//
//void Shader::vec4(const char* name, float value[4]) {
//	glUniform4fv(glGetUniformLocation(shaderProgram, name), 1, value);
//}
//
//
//void Shader::setInt(const char* name, int value) {
//	glUniform1i(glGetUniformLocation(shaderProgram, name), value);
//}
//
//void Shader::ivec2(const char* name, int value[2]) {
//	glUniform2iv(glGetUniformLocation(shaderProgram, name), 1, value);
//}
//
//void Shader::ivec3(const char* name, int value[3]) {
//	glUniform3iv(glGetUniformLocation(shaderProgram, name), 1, value);
//}
//
//void Shader::ivec4(const char* name, int value[4]) {
//	glUniform4iv(glGetUniformLocation(shaderProgram, name), 1, value);
//}
//
//
//void Shader::setUint(const char* name, unsigned int value) {
//	glUniform1ui(glGetUniformLocation(shaderProgram, name), value);
//}
//
//void Shader::uvec2(const char* name, unsigned int value[2]) {
//	glUniform2uiv(glGetUniformLocation(shaderProgram, name), 1, value);
//}
//
//void Shader::uvec3(const char* name, unsigned int value[3]) {
//	glUniform3uiv(glGetUniformLocation(shaderProgram, name), 1, value);
//}
//
//void Shader::uvec4(const char* name, unsigned int value[4]) {
//	glUniform4uiv(glGetUniformLocation(shaderProgram, name), 1, value);
//}
//
//void Shader::mat2(const char* name, float value[4]) { // requires matrix in column major
//	glUniformMatrix2fv(glGetUniformLocation(shaderProgram, name), 1, GL_FALSE, value);
//}
//
//void Shader::mat3(const char* name, float value[9]) {
//	glUniformMatrix3fv(glGetUniformLocation(shaderProgram, name), 1, GL_FALSE, value);
//}
//
//void Shader::mat4(const char* name, float value[16]) {
//	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name), 1, GL_FALSE, value);
//}


// source file for shader class
#include "shaders.h"

constexpr uint16_t log_length = 512;


// constructor
Shader::Shader(const char* vertexShaderPath, const char* fragmentShaderPath, bool& status) {
	// initialize success to true before reset of function
	status = true;

	// strings to read file contents into
	std::string vertexShaderString, fragmentShaderString;

	// read files to string checking return values
	if (
		!readFileToString(vertexShaderPath, vertexShaderString) ||
		!readFileToString(fragmentShaderPath, fragmentShaderString))
	{
		status = false;
		return;
	}

	// convert C++ strings to C const char* pointers
	const char* vertexShaderCstr = vertexShaderString.c_str();
	const char* fragmentShaderCstr = fragmentShaderString.c_str();

	// create empty shader objects
	uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
	uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	// check objects were created
	if (!vertexShader || !fragmentShader) {
		std::cout << "Failed to create Shader objects\n";
		status = false;
		goto clean;
	}

	// attach source code to shader objects
	glShaderSource(vertexShader, 1, &vertexShaderCstr, NULL);
	glShaderSource(fragmentShader, 1, &fragmentShaderCstr, NULL);

	// compile shader source code
	glCompileShader(vertexShader);
	glCompileShader(fragmentShader);

	// check for any compile errors and post errors in console
	int32_t vertexStatus, fragmentStatus;

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexStatus);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragmentStatus);

	// log errors to user
	// do not immediately return if errors detected
	if (!vertexStatus) {
		char log[log_length];
		// get full error message
		glGetShaderInfoLog(vertexShader, log_length, NULL, log);
		std::cout << "Vertex shader compile failed:\n" << log << "\n";
		status = false;
	}
	if (!fragmentStatus) {
		char log[log_length];
		// get full error message
		glGetShaderInfoLog(fragmentShader, log_length, NULL, log);
		std::cout << "Fragment shader compile failed:\n" << log << "\n";
		status = false;
	}

	// once errors of both programs have been reported,
	// return
	if (!status) goto clean;


	// create shader program object
	shaderProgram = glCreateProgram();
	// return if null
	if (!shaderProgram) {
		std::cout << "Failed to create Shader program object\n";
		status = false;
		goto clean;
	}

	// attach compiled files to program
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	// link program, then check for errors
	glLinkProgram(shaderProgram);

	int32_t linkStatus;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkStatus);

	if (!linkStatus) {
		char log[log_length];
		std::cout << "Shader program linking failed:\n" << log << "\n";
		status = false;
		goto clean;
	}

	// I know goto is 'bad practice'
	// but I find it extremely useful for jumping to a cleanup section
clean:
	// clean up before exiting
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

// program destructor
// detetes shader program
Shader::~Shader() {
	glDeleteProgram(shaderProgram);
}


void Shader::use() {
	this;
	glUseProgram(shaderProgram);
}


// procedures for setting uniforms on the shader program
// each of these procedures gets the location of the selected uniform
// and then sets its value
// all values are single precision

void Shader::setFloat(const char* name, float value) {
	glUniform1f(glGetUniformLocation(shaderProgram, name), value);
}

void Shader::setVec2(const char* name, float* value) {
	glUniform2fv(glGetUniformLocation(shaderProgram, name), 1, value);
}

void Shader::setVec3(const char* name, float* value) {
	glUniform3fv(glGetUniformLocation(shaderProgram, name), 1, value);
}

void Shader::setVec4(const char* name, float* value) {
	glUniform4fv(glGetUniformLocation(shaderProgram, name), 1, value);
}


void Shader::setInt(const char* name, int32_t value) {
	glUniform1i(glGetUniformLocation(shaderProgram, name), value);
}

void Shader::setIvec2(const char* name, int32_t* value) {
	glUniform2iv(glGetUniformLocation(shaderProgram, name), 1, value);
}

void Shader::setIvec3(const char* name, int32_t* value) {
	glUniform3iv(glGetUniformLocation(shaderProgram, name), 1, value);
}

void Shader::setIvec4(const char* name, int32_t* value) {
	glUniform4iv(glGetUniformLocation(shaderProgram, name), 1, value);
}


void Shader::setUint(const char* name, uint32_t value) {
	glUniform1ui(glGetUniformLocation(shaderProgram, name), value);
}

void Shader::setUvec2(const char* name, uint32_t* value) {
	glUniform2uiv(glGetUniformLocation(shaderProgram, name), 1, value);
}

void Shader::setUvec3(const char* name, uint32_t* value) {
	glUniform3uiv(glGetUniformLocation(shaderProgram, name), 1, value);
}

void Shader::setUvec4(const char* name, uint32_t* value) {
	glUniform4uiv(glGetUniformLocation(shaderProgram, name), 1, value);
}


void Shader::setMat2(const char* name, float* value) {
	glUniformMatrix2fv(glGetUniformLocation(shaderProgram, name), 1, GL_FALSE, value);
}

void Shader::setMat3(const char* name, float* value) {
	glUniformMatrix3fv(glGetUniformLocation(shaderProgram, name), 1, GL_FALSE, value);
}

void Shader::setMat4(const char* name, float* value) {
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name), 1, GL_FALSE, value);
}