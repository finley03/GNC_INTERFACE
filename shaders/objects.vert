#version 460 core

// inputs in vertex buffer object
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

// matrix uniforms
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// outputs to other shaders
out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;


void main() {
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	TexCoord = aTexCoord;
	Normal = mat3(model) * aNormal;
	FragPos = vec3(model * vec4(aPos, 1.0));
}
