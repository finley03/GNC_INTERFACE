// csys vertex shader

#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;

uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec4 Color;

void main() {
	gl_Position = projection * view * vec4(aPos, 1.0);
//	gl_Position = vec4(aPos, 1.0);
	FragPos = aPos;
	Color = aColor;
}
