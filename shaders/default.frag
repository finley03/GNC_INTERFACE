#version 460 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 color;
uniform sampler2D texture0;
uniform bool textureDiffuse;

out vec4 FragColor;

void main() {
	if (textureDiffuse) {
		FragColor = texture(texture0, TexCoord);
	}
	else {
		FragColor = vec4(color, 1.0);
	}
}
