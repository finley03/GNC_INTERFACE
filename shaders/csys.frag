// csys fragment shader

#version 460 core

in vec3 FragPos;
in vec4 Color;

out vec4 FragColor;

void main() {
	FragColor = Color;
}

