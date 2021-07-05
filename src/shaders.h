#ifndef SHADERS_H
#define SHADERS_H

#include <string>
#include <iostream>

#include "util.h"
#include "glad/glad.h"

class Shader {
private:
	unsigned int shaderProgram = 0;

public:
	Shader(const char* vertexShaderPath, const char* fragmentShaderPath, int &success);

	void use();

	void close();

	// set uniforms of various types
	void setFloat(const char* name, float value);
	void vec2(const char* name, float value[2]);
	void vec3(const char* name, float value[3]);
	void vec4(const char* name, float value[4]);

	void setInt(const char* name, int value);
	void ivec2(const char* name, int value[2]);
	void ivec3(const char* name, int value[3]);
	void ivec4(const char* name, int value[4]);

	void setUint(const char* name, unsigned int value);
	void uvec2(const char* name, unsigned int value[2]);
	void uvec3(const char* name, unsigned int value[3]);
	void uvec4(const char* name, unsigned int value[4]);

	void mat2(const char* name, float value[4]);
	void mat3(const char* name, float value[9]);
	void mat4(const char* name, float value[16]);
};

#endif