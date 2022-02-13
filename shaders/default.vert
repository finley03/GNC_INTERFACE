#version 460 core

// inputs of vertex buffer object
// same layout as defined in object mesh
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

// matrix inputs
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// outputs to other shaders
out vec3 Position;
out vec3 Normal;
out vec2 TexCoord;

// main function
void main() {
	// calculate vertex position on screen
	gl_Position = projection * view * model * vec4(aPosition, 1.0);
	// assign values
	Position = vec3(model * vec4(aPosition, 1.0));
	Normal = aNormal;
	TexCoord = aTexCoord;
}
