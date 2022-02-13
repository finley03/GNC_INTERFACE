// Screen vertex shader

#version 460 core

// assign data at first buffer pointer to aPos
layout (location = 0) in vec3 aPos;

// camera position
uniform vec3 position;
// camera target
uniform vec3 target;

// transformation matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// output fragment position
out vec3 FragPos;

void main() {
	// calculate distance from positon to target
	float distance = length(target - position);

	// modify position of vertex
	vec3 pos = aPos * distance * 100;

	// calculate vertex position and fragment position
	gl_Position = projection * view * model * vec4(pos, 1.0);
	FragPos = (model * vec4(pos, 1.0)).xyz;
}
