// source file for camera class

#include "camera.h"

void Camera::setPosition(float* position) {
	// passed parameters take precedence over class attributes
	// therefore this is used to access class attributes
	this->position = glm::make_vec3(position);
}

void Camera::setTarget(float* target) {
	this->target = glm::make_vec3(target);
}

void Camera::setUp(float* up) {
	this->up = glm::make_vec2(up);
}

void Camera::setRotation(float rotation) {
	// use trigonmetric functions to find up direction
	up = glm::vec2(-sin(glm::radians(rotation)), cos(glm::radians(rotation)));
	this->rotation = rotation;
}

void Camera::setFov(float fov) {
	this->fov = fov;
}

void Camera::setScreen(UINT_T width, UINT_T height) {
	this->width = width;
	this->height = height;
}

void Camera::setRange(float near, float far) {
	nearDistance = near;
	farDistance = far;
}

void Camera::setTargetTrack(float* target) {
	glm::vec3 delta = glm::make_vec3(target) - this->target;
	this->target += delta;
	this->position += delta;
}


void Camera::getPosition(float* writeback) {
	writeback[0] = position.x;
	writeback[1] = position.y;
	writeback[2] = position.z;
}

void Camera::getTarget(float* writeback) {
	writeback[0] = target.x;
	writeback[1] = target.y;
	writeback[2] = target.z;
}

void Camera::getUp(float* writeback) {
	writeback[0] = up.x;
	writeback[1] = up.y;
}

float Camera::getRotation() {
	return rotation;
}

float Camera::getFov() {
	return fov;
}

glm::mat4 Camera::getViewMatrix() {
	return view;
}

glm::mat4 Camera::getProjectionMatrix() {
	return projection;
}


void Camera::translate(float* delta) {
	glm::vec3 deltavec = glm::make_vec3(delta);
	position += deltavec;
	target += deltavec;
}

void Camera::translateTarget(float* delta) {
	target += glm::make_vec3(delta);
}

void Camera::translatePosition(float* delta) {
	position += glm::make_vec3(delta);
}


void Camera::mouseRevolve(float mouseX, float mouseY) {
	// vector from target to camera
	glm::vec3 cameraTargetVector = target - position;
	// create identity rotation matrix
	glm::mat4 rotateMatrix = glm::mat4(1.0f);
	// down direction in XYZ coordinates
	glm::vec3 down = glm::vec3(0.0f, -1.0f, 0.0f);
	// rotate camera in Y direction
	// rotates by calculated value anticlockwise
	// around vector calculated as the cross product between
	// the cameraTargetVector and down
	glm::vec3 Yaxis = glm::normalize(glm::cross(cameraTargetVector, down));
	rotateMatrix = glm::rotate(rotateMatrix, (mouseY / height) * 2.0f, Yaxis);
	// rotate camera in X direction around down axis
	rotateMatrix = glm::rotate(rotateMatrix, (mouseX / height) * 2.0f, down);

	// translate position to target
	position += cameraTargetVector;
	// translate position to new position
	position -= glm::vec3(rotateMatrix * glm::vec4(cameraTargetVector, 1.0f));
}

void Camera::mouseZoom(float mouseZoom) {
	glm::vec3 cameraTargetVector = target - position;
	// get scale factor of zoom
	float scale = pow(1.25, -mouseZoom);
	// adjust position
	position += cameraTargetVector * (1 - scale);
}

void Camera::mouseTranslate(float mouseX, float mouseY) {
	mouseX = -mouseX * 2 / height;
	mouseY = mouseY * 2 / height;

	// vector from target to camera
	glm::vec3 cameraTargetVector = target - position;
	// calculate length of cameratargetvector
	float cameraTargetLength = glm::length(cameraTargetVector);

	glm::vec3 directionX, directionY;

	// set plane direction vectors
	directionX = glm::normalize(glm::cross(cameraTargetVector, glm::vec3(0.0f, 1.0f, 0.0f)));
	directionY = glm::normalize(glm::cross(directionX, cameraTargetVector));

	// initialize position deltas
	glm::vec3 deltaY, deltaX;

	// set position deltas
	deltaX = directionX * mouseX * cameraTargetLength * sin(glm::radians(fov / 2));
	deltaY = directionY * mouseY * cameraTargetLength * sin(glm::radians(fov / 2));

	position = position + deltaX + deltaY;
	target = target + deltaX + deltaY;
}

void Camera::calculateViewMatrix() {
	view = glm::lookAt(position, target, glm::vec3(up, 0.0f));
}

void Camera::calculateProjectionMatrix() {
	projection = glm::perspective(glm::radians(fov), (float)width / height, nearDistance, farDistance);
}