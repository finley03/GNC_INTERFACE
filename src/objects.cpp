#include "objects.h"


Objects* objects;


Objects::Objects(Camera* camera_ptr, int &success) : shader(vertexFilePath, fragmentFilePath, shaderStatus) {
	if (!shaderStatus) {
		success = false;
		return;
	}

	camera = camera_ptr;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
}


int Objects::newObject(std::string filePath, int& success) {
	success = false;

	// create object data type
	Data object;

	// generate vertex buffer object for object
	glGenBuffers(1, &object.VBO);

	// generate model mesh from file
	int meshStatus;
	std::vector<float> mesh = OBJgenMesh(filePath, meshStatus, object.size);
	if (!meshStatus) return -1;


	// get material data for object
	object.materials = OBJgetMaterialData(filePath, meshStatus);
	//if (!meshStatus) return -1;

	// get material indexes for object
	object.materialIndex = OBJgetMaterialIndexes(filePath, meshStatus);
	//if (!meshStatus) return -1;


	// buffer data to gpu
	glBindBuffer(GL_ARRAY_BUFFER, object.VBO);
	glBufferData(GL_ARRAY_BUFFER, mesh.size() * sizeof(float), &mesh[0], GL_STATIC_DRAW);


	int objectIndex = objects.size();
	objects.push_back(object);


	success = true;

	return objectIndex;

}


void Objects::setPosition(int objectHandle, float* newPosition) {
	for (int i = 0; i < 3; ++i) {
		objects[objectHandle].position[i] = newPosition[i];
	}
}

void Objects::setOrientation(int objectHandle, float* newOrientation) {
	for (int i = 0; i < 3; ++i) {
		objects[objectHandle].orientation[i] = newOrientation[i];
	}
}

void Objects::setScale(int objectHandle, float newScale) {
	objects[objectHandle].scale = newScale;
}

void Objects::getPosition(int objectHandle, float* writeback) {
	for (int i = 0; i < 3; ++i) {
		writeback[i] = objects[objectHandle].position[i];
	}
}

void Objects::getOrientation(int objectHandle, float* writeback) {
	for (int i = 0; i < 3; ++i) {
		writeback[i] = objects[objectHandle].orientation[i];
	}
}

float Objects::getScale(int objectHandle) {
	return objects[objectHandle].scale;
}


void Objects::draw(int objectHandle) {
	// set vertex attribute pointer
	shader.use();


	// object transformations
	
	glm::mat4 model = glm::mat4(1.0);
	// translate according to OpenGL local frame
	model = glm::translate(model, glm::make_vec3(objects[objectHandle].position));
	// rotate according to euler angles z/y/x in north east down frame
	// north is -
	model = glm::rotate(model, glm::radians(objects[objectHandle].orientation[2]), glm::vec3(0.0f, -1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(objects[objectHandle].orientation[1]), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(objects[objectHandle].orientation[0]), glm::vec3(0.0f, 0.0f, -1.0f));

	float scale = objects[objectHandle].scale;
	model = glm::scale(model, glm::vec3(scale, scale, scale));


	glm::mat4 view = camera->getViewMatrix();
	glm::mat4 projection = camera->getProjectionMatrix();

	shader.mat4("model", glm::value_ptr(model));
	shader.mat4("view", glm::value_ptr(view));
	shader.mat4("projection", glm::value_ptr(projection));

	//float color[] = { 0.2, 0.8, 0.1, 1.0 };
	//shader.vec3("color", color);


	glBindBuffer(GL_ARRAY_BUFFER, objects[objectHandle].VBO); // bind vbo of shape

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // vertex attribute pointer
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // vertex normal attribute pointer
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // texture attribute pointer
	glEnableVertexAttribArray(2);


	// load materials
	// increment 2 because there are two indexes for each material, startindex and materialindex
	for (int i = 0; i < objects[objectHandle].materialIndex.size(); i += 2) {
		// 3 because triangles
		int startVertex = objects[objectHandle].materialIndex[i] * 3;

		int nrVertices;
		if (i + 2 >= objects[objectHandle].materialIndex.size()) { // check if i is out of bounds
			nrVertices = objects[objectHandle].size - objects[objectHandle].materialIndex[i] * 3;
		}
		else {
			nrVertices = objects[objectHandle].materialIndex[i + 2] * 3 - objects[objectHandle].materialIndex[i] * 3;
		}


		shader.vec3("color", objects[objectHandle].materials[objects[objectHandle].materialIndex[i + 1]].color);

		glDrawArrays(GL_TRIANGLES, startVertex, nrVertices);
	}
}