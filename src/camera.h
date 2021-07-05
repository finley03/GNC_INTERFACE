#ifndef CAMERA_H
#define CAMERA_H

#include <cmath>
#include <iostream>
#include <glm/glm.hpp> // gl mathematics library
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera {
private:
	glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 10.0f);

	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	float fov = 45;
	int height = 600;
	int width = 800;
	float nearDistance = 3;
	float farDistance = 100;


public:
	void translate(float* delta);
	void translateTarget(float* delta);
	void translatePosition(float* delta);

	void setPosition(float* newPosition);
	void setTarget(float* newTarget);
	void setTargetTrack(float* newTarget);

	void setProjection(float newFov, int newWidth, int newHeight, float newNear, float newFar);
	void setFov(float newFov);
	void setScreen(int newWidth, int newHeight);
	void setRange(float newNear, float newFar);

	void getPosition(float* writeback);
	void getTarget(float* writeback);
	float getFov();

	void calculateViewMatrix();
	void calculateProjectionMatrix();
	// projection matrix is calculated automatically

	glm::mat4 getViewMatrix();
	glm::mat4 getProjectionMatrix();

	// mouse operations (floating point for futureproofing for other features)
	void mouseRevolve(float mouseX, float mouseY);
	void mouseZoom(float mouseZoom);
	void mouseTranslate(float mouseX, float mouseY);
};


extern Camera camera;


#endif