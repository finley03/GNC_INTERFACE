#ifndef OBJECTS_H
#define OBJECTS_H

#include <string>
#include <vector>
#include <iostream>

#include <glm/glm.hpp> // gl mathematics library
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shaders.h"
#include "obj.h"
#include "camera.h"

class Objects {
private:
	unsigned int VAO = 0;

	const char* vertexFilePath = "./shaders/objects.vert";
	const char* fragmentFilePath = "./shaders/objects.frag";

	int shaderStatus;

	Shader shader;


	struct Data {
		unsigned int VBO; // vertex buffer object containing mesh

		std::vector<Material> materials; // vector of materials used in object

		std::vector<unsigned int> materialIndex;

		bool display = true;

		float scale = 1; // scale of object to be drawn

		float position[3] = { 0 };
		float orientation[3] = { 0 };

		unsigned int size; // number of vertices
	};

	std::vector<Data> objects; // vector of objects


	// pointer to camera class
	Camera* camera;


public:
	Objects(Camera* camera_ptr, int &success);

	// returns index in vector of object
	int newObject(std::string filePath, int& success);

	void setPosition(int objectHandle, float* newPosition);
	void setOrientation(int objectHandle, float* newOrientation);
	void setScale(int objectHandle, float newScale);

	void getPosition(int objectHandle, float* writeback);
	void getOrientation(int objectHandle, float* writeback);
	float getScale(int objectHandle);

	void draw(int objectHandle);

};


extern Objects* objects;


#endif