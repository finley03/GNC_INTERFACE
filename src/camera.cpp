#include "camera.h"


Camera camera;


void Camera::translate(float* delta) {
	//position.x += delta[0];
	//position.y += delta[1];
	//position.z += delta[2];
	//target.x += delta[0];
	//target.y += delta[1];
	//target.z += delta[2];
	glm::vec3 deltavec = glm::make_vec3(delta);
	position += deltavec;
	target += deltavec;
}

void Camera::translateTarget(float* delta) {
	//target.x += delta[0];
	//target.y += delta[1];
	//target.z += delta[2];
	target += glm::make_vec3(delta);
}

void Camera::translatePosition(float* delta) {
	//position.x += delta[0];
	//position.y += delta[1];
	//position.z += delta[2];
	position += glm::make_vec3(delta);
}

void Camera::setPosition(float* newPosition) {
	//position.x = newPosition[0];
	//position.y = newPosition[1];
	//position.z = newPosition[2];
	position = glm::make_vec3(newPosition);
}

void Camera::setTarget(float* newTarget) {
	//target.x = newTarget[0];
	//target.y = newTarget[1];
	//target.z = newTarget[2];
	target = glm::make_vec3(newTarget);
}

void Camera::setTargetTrack(float* newTarget) {
	glm::vec3 delta = glm::make_vec3(newTarget) - target;
	target += delta;
	position += delta;
}

void Camera::setProjection(float newFov, int newWidth, int newHeight, float newNear, float newFar) {
	fov = newFov;
	width = newWidth;
	height = newHeight;
	nearDistance = newNear;
	farDistance = newFar;
}

void Camera::setFov(float newFov) {
	fov = newFov;
}

void Camera::setScreen(int newWidth, int newHeight) {
	height = newHeight;
	width = newWidth;
}

void Camera::setRange(float newNear, float newFar) {
	nearDistance = newNear;
	farDistance = newFar;
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

float Camera::getFov() {
	return fov;
}

void Camera::calculateViewMatrix() {
	view = glm::lookAt(position, target, glm::vec3(0.0f, 1.0f, 0.0f));
}

void Camera::calculateProjectionMatrix() {
	projection = glm::perspective(glm::radians(fov), (float) width / height, nearDistance, farDistance);
}

glm::mat4 Camera::getViewMatrix() {
	return view;
}

glm::mat4 Camera::getProjectionMatrix() {
	return projection;
}

void Camera::mouseRevolve(float mouseX, float mouseY) {
	// vector from target to camera:
	glm::vec3 cameraTargetVector = target - position;

	glm::mat4 rotateMatrix = glm::mat4(1.0f);
	rotateMatrix = glm::rotate(rotateMatrix, (mouseY / height) * 2.0f, glm::normalize(glm::cross(cameraTargetVector, glm::vec3(0.0f, -1.0f, 0.0f))));
	rotateMatrix = glm::rotate(rotateMatrix, (mouseX / height) * 2.0f, glm::vec3(0.0f, -1.0f, 0.0f));

	// translate position to target
	position = position + cameraTargetVector;
	// translate positon to new position;
	position = position - glm::vec3(rotateMatrix * glm::vec4(cameraTargetVector, 1.0f));
}

void Camera::mouseZoom(float mouseZoom) {
	glm::vec3 cameraTargetVector = target - position;

	float scale = pow(1.25, -mouseZoom);

	position += cameraTargetVector * (1 - scale);
}

void Camera::mouseTranslate(float mouseX, float mouseY) {
	mouseX = -mouseX * 2 / height;
	mouseY = mouseY * 2 / height;

	// vector from target to camera:
	glm::vec3 cameraTargetVector = target - position;
	float cameraTargetLength = glm::length(cameraTargetVector);

	glm::vec3 directionY, directionX;

	directionX = glm::normalize(glm::cross(cameraTargetVector, glm::vec3(0.0f, 1.0f, 0.0f)));
	directionY = glm::normalize(glm::cross(directionX, cameraTargetVector));

	glm::vec3 deltaY = glm::vec3(0.0f), deltaX = glm::vec3(0.0f);

	deltaX = directionX * mouseX * cameraTargetLength * sin(glm::radians(fov / 2));
	deltaY = directionY * mouseY * cameraTargetLength * sin(glm::radians(fov / 2));

	position = position + deltaX + deltaY;
	target = target + deltaX + deltaY;
}