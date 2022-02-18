// header file for camera class
// handles camera position and orientation
// and generates view and projection matrices
// glm types are kept to the class
// the public interface uses (mostly) standard types

#ifndef CAMERA_H
#define CAMERA_H

#include "util.h"
#include <glm/glm.hpp> // gl mathematics library
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera {
private:
	// values must be initialized

	// point in space that the camera points to
	glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
	// position in space of the camera
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 10.0f);
	// up orientation
	glm::vec2 up = glm::vec2(0.0f, 1.0f);
	// view matrix
	// deals with camera position and target
	// initialize to identity matrix
	glm::mat4 view = glm::mat4(1.0f);
	// projection matrix
	// handles projection of vertices onto screen
	glm::mat4 projection = glm::mat4(1.0f);

	// field of view of the camera (vertical)
	float fov = 45;
	// rotation angle
	// only stored so I don't have to calculate it
	float rotation;
	// dimensions of screen
	UINT_T width = 1000;
	UINT_T height = 800;
	// near distance of depth buffer
	float nearDistance = 1.0f;
	// far distance of frame buffer
	float farDistance = 1000000.0f;

public:
	// Setter for position attribute
	void setPosition(float* position);
	// Setter for target attribute
	void setTarget(float* position);
	// Setter for up attribute
	void setUp(float* position);
	// Procedure that calculates the up attribute
	// Angle is anticlockwise in degrees
	void setRotation(float angle);
	// Setter for field of view in degrees
	void setFov(float fov);
	// Setter for screen size
	void setScreen(UINT_T width, UINT_T height);
	// Setter for depth buffer range
	void setRange(float near, float far);
	// Procedure that sets target maintaining orientation
	void setTargetTrack(float* target);

	// Position getter
	void getPosition(float* writeback);
	// Target getter
	void getTarget(float* writeback);
	// up direction getter
	void getUp(float* writeback);
	// Up rotation getter
	float getRotation();
	// FOV getter
	float getFov();
	// View matrix getter
	glm::mat4 getViewMatrix();
	glm::mat4 getProjectionMatrix();

	// Translate position and target
	void translate(float* delta);
	// Translate target
	void translateTarget(float* delta);
	// translate position
	void translatePosition(float* delta);

	// Revolve around target based on mouse deltas
	void mouseRevolve(float mouseX, float mouseY);
	// Zoom off scrollwheel input
	void mouseZoom(float mouseZoom);
	// Translate based on mouse deltas
	void mouseTranslate(float mouseX, float mouseY);

	// Procedure to calculate view matrix
	// using stored parameters
	void calculateViewMatrix();
	// Procedure to calcaulte projection matrix
	// using stored parameters
	void calculateProjectionMatrix();
};

#endif