// header file for coordinate system class

#ifndef CSYS_H
#define CSYS_H

#include "util.h"
#include "camera.h"
#include "shaders.h"
#include "screen.h"

class Csys {
private:
	INT_T width;
	INT_T height;

	// vertex buffer object
	unsigned int VBO;
	// paths to shaders
	const char* csysVert = "./shaders/csys.vert";
	const char* csysFrag = "./shaders/csys.frag";
	bool shaderStatus;

	// shader object
	Shader shader;

	// screen object
	Screen screen;
	bool screenStatus;

	uint32_t texHandle;

	// pointer to camera object
	Camera* camera;

	// data for CSYS
	// 3 floats for position, 4 for color
	const float vertices[42] = {
		0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
	};

	glm::mat4 view;
	// store orthographic projection matrix here
	glm::mat4 projection;
public:
	// constructor
	Csys(Camera* camera, INT_T width, INT_T height);

	// destructor
	~Csys();

	// render coordinate system to screen
	void draw();

	// show window with pre-rendered texture
	void showWindow();
	// get handle to pre-rendered texture
	uint32_t getTexHandle();

	// get view matrix for csys
	glm::mat4 getViewMatrix();
	// get projection matrix for csys
	glm::mat4 getProjectionMatrix();
};

#endif