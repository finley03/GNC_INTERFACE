#include "grid.h"


Grid* grid;


Grid::Grid() : shader(gridVert, gridFrag, shaderStatus) {
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(float), &vertices[0], GL_STATIC_DRAW);
}

void Grid::setPosition(float* position) {
	this->position.x = position[0];
	this->position.y = position[1];
	this->position.z = position[2];
}

void Grid::setSize(float scale) {
	this->scale = scale;
}

void Grid::calculateModelMatrix() {
	model = glm::mat4(1.0f);

	model = glm::translate(model, position);
	model = glm::scale(model, glm::vec3(scale));
}


void Grid::draw() {
	shader.use();
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // vertex attribute pointer
	glEnableVertexAttribArray(0);

	glm::mat4 view = camera.getViewMatrix();
	glm::mat4 projection = camera.getProjectionMatrix();
	shader.mat4("view", glm::value_ptr(view));
	shader.mat4("projection", glm::value_ptr(projection));
	shader.mat4("model", glm::value_ptr(model));
	float cameraPosition[3];
	camera.getPosition(cameraPosition);
	shader.vec3("position", cameraPosition);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}