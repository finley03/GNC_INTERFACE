// Screen fragment shader

#version 460 core

in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D scrTex;

void main() {
	FragColor = texture(scrTex, texCoord);
}
