#version 460 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

// color input
uniform vec3 color;

out vec4 FragColor;

void main() {
	FragColor = vec4(color, 1.0);
}
