//#ifndef SHADERS_H
//#define SHADERS_H
//
//#include <string>
//#include <iostream>
//
//#include "util.h"
//#include "glad/glad.h"
//
//class Shader {
//private:
//	unsigned int shaderProgram = 0;
//
//public:
//	Shader(const char* vertexShaderPath, const char* fragmentShaderPath, int &success);
//
//	void use();
//
//	void close();
//
//	// set uniforms of various types
//	void setFloat(const char* name, float value);
//	void vec2(const char* name, float value[2]);
//	void vec3(const char* name, float value[3]);
//	void vec4(const char* name, float value[4]);
//
//	void setInt(const char* name, int value);
//	void ivec2(const char* name, int value[2]);
//	void ivec3(const char* name, int value[3]);
//	void ivec4(const char* name, int value[4]);
//
//	void setUint(const char* name, unsigned int value);
//	void uvec2(const char* name, unsigned int value[2]);
//	void uvec3(const char* name, unsigned int value[3]);
//	void uvec4(const char* name, unsigned int value[4]);
//
//	void mat2(const char* name, float value[4]);
//	void mat3(const char* name, float value[9]);
//	void mat4(const char* name, float value[16]);
//};
//
//#endif


// header file declaring shader class
// each instance of this class manages a shader program

#ifndef SHADERS_H
#define SHADERS_H

#include <glad/glad.h>
#include "util.h"

// Responsible for creating and handling a shader program.
class Shader {
private:
	// 32bit unsigned integer that acts as
	// handle for shader program
	uint32_t shaderProgram = 0;

public:
	// Constructor takes paths to vertex shader and fragment shader
	// in const char* type. A boolean variable passed to success will
	// be updated to indicate weather the function has been successful.
	Shader(const char* vertexShaderPath, const char* fragmentShaderPath, bool& status);

	// destructor will delete shader program from GPU
	~Shader();

	// Tells OpenGL to use this shader program
	void use();

	// Set a floating point uniform
	void setFloat(const char* name, float value);
	// Set a floating point uniform vector
	void setVec2(const char* name, float* value);
	// Set a floating point uniform vector
	void setVec3(const char* name, float* value);
	// Set a floating point uniform vector
	void setVec4(const char* name, float* value);

	// Set a signed integer uniform
	void setInt(const char* name, int32_t value);
	// Set a signed integer uniform vector
	void setIvec2(const char* name, int32_t* value);
	// Set a signed integer uniform vector
	void setIvec3(const char* name, int32_t* value);
	// Set a signed integer uniform vector
	void setIvec4(const char* name, int32_t* value);

	// Set an unsigned integer uniform
	void setUint(const char* name, uint32_t value);
	// Set an unsigned integer uniform vector
	void setUvec2(const char* name, uint32_t* value);
	// Set an unsigned integer uniform vector
	void setUvec3(const char* name, uint32_t* value);
	// Set an unsigned integer uniform vector
	void setUvec4(const char* name, uint32_t* value);

	// Set a floating point matrix uniform
	void setMat2(const char* name, float* value);
	// Set a floating point matrix uniform
	void setMat3(const char* name, float* value);
	// Set a floating point matrix uniform
	void setMat4(const char* name, float* value);
};

#endif