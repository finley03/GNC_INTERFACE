//#include "objects.h"
//
//
//Objects* objects;
//
//
//Objects::Objects(Camera* camera_ptr, int &success) : shader(vertexFilePath, fragmentFilePath, shaderStatus) {
//	if (!shaderStatus) {
//		success = false;
//		return;
//	}
//
//	camera = camera_ptr;
//
//	glGenVertexArrays(1, &VAO);
//	glBindVertexArray(VAO);
//}
//
//
//int Objects::newObject(std::string filePath, int& success) {
//	success = false;
//
//	// create object data type
//	Data object;
//
//	// generate vertex buffer object for object
//	glGenBuffers(1, &object.VBO);
//
//	// generate model mesh from file
//	int meshStatus;
//	std::vector<float> mesh = OBJgenMesh(filePath, meshStatus, object.size);
//	if (!meshStatus) return -1;
//
//
//	// get material data for object
//	object.materials = OBJgetMaterialData(filePath, meshStatus);
//	//if (!meshStatus) return -1;
//
//	// get material indexes for object
//	object.materialIndex = OBJgetMaterialIndexes(filePath, meshStatus);
//	//if (!meshStatus) return -1;
//
//
//	// buffer data to gpu
//	glBindBuffer(GL_ARRAY_BUFFER, object.VBO);
//	glBufferData(GL_ARRAY_BUFFER, mesh.size() * sizeof(float), &mesh[0], GL_STATIC_DRAW);
//
//
//	int objectIndex = objects.size();
//	objects.push_back(object);
//
//
//	success = true;
//
//	return objectIndex;
//
//}
//
//
//void Objects::setPosition(int objectHandle, float* newPosition) {
//	for (int i = 0; i < 3; ++i) {
//		objects[objectHandle].position[i] = newPosition[i];
//	}
//}
//
//void Objects::setOrientation(int objectHandle, float* newOrientation) {
//	for (int i = 0; i < 3; ++i) {
//		objects[objectHandle].orientation[i] = newOrientation[i];
//	}
//}
//
//void Objects::setScale(int objectHandle, float newScale) {
//	objects[objectHandle].scale = newScale;
//}
//
//void Objects::getPosition(int objectHandle, float* writeback) {
//	for (int i = 0; i < 3; ++i) {
//		writeback[i] = objects[objectHandle].position[i];
//	}
//}
//
//void Objects::getOrientation(int objectHandle, float* writeback) {
//	for (int i = 0; i < 3; ++i) {
//		writeback[i] = objects[objectHandle].orientation[i];
//	}
//}
//
//float Objects::getScale(int objectHandle) {
//	return objects[objectHandle].scale;
//}
//
//
//void Objects::draw(int objectHandle) {
//	// set vertex attribute pointer
//	shader.use();
//
//
//	// object transformations
//	
//	glm::mat4 model = glm::mat4(1.0);
//	// translate according to OpenGL local frame
//	model = glm::translate(model, glm::make_vec3(objects[objectHandle].position));
//	// rotate according to euler angles z/y/x in north east down frame
//	// north is -
//	model = glm::rotate(model, glm::radians(objects[objectHandle].orientation[2]), glm::vec3(0.0f, -1.0f, 0.0f));
//	model = glm::rotate(model, glm::radians(objects[objectHandle].orientation[1]), glm::vec3(1.0f, 0.0f, 0.0f));
//	model = glm::rotate(model, glm::radians(objects[objectHandle].orientation[0]), glm::vec3(0.0f, 0.0f, -1.0f));
//
//	float scale = objects[objectHandle].scale;
//	model = glm::scale(model, glm::vec3(scale, scale, scale));
//
//
//	glm::mat4 view = camera->getViewMatrix();
//	glm::mat4 projection = camera->getProjectionMatrix();
//
//	shader.mat4("model", glm::value_ptr(model));
//	shader.mat4("view", glm::value_ptr(view));
//	shader.mat4("projection", glm::value_ptr(projection));
//
//	//float color[] = { 0.2, 0.8, 0.1, 1.0 };
//	//shader.vec3("color", color);
//
//
//	glBindBuffer(GL_ARRAY_BUFFER, objects[objectHandle].VBO); // bind vbo of shape
//
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // vertex attribute pointer
//	glEnableVertexAttribArray(0);
//
//	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // vertex normal attribute pointer
//	glEnableVertexAttribArray(1);
//
//	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // texture attribute pointer
//	glEnableVertexAttribArray(2);
//
//
//	// load materials
//	// increment 2 because there are two indexes for each material, startindex and materialindex
//	for (int i = 0; i < objects[objectHandle].materialIndex.size(); i += 2) {
//		// 3 because triangles
//		int startVertex = objects[objectHandle].materialIndex[i] * 3;
//
//		int nrVertices;
//		if (i + 2 >= objects[objectHandle].materialIndex.size()) { // check if i is out of bounds
//			nrVertices = objects[objectHandle].size - objects[objectHandle].materialIndex[i] * 3;
//		}
//		else {
//			nrVertices = objects[objectHandle].materialIndex[i + 2] * 3 - objects[objectHandle].materialIndex[i] * 3;
//		}
//
//
//		shader.vec3("color", objects[objectHandle].materials[objects[objectHandle].materialIndex[i + 1]].color);
//
//		glDrawArrays(GL_TRIANGLES, startVertex, nrVertices);
//	}
//}


// source file for objects class

#include "objects.h"
//#include "util.h"
//#include "obj.h"
//#include "simulation.h"
extern "C" {
#include "mat.h"
}

#include <glm/glm.hpp> // gl mathematics library
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Objects* objects;

//#define STB_IMAGE_WRITE_IMPLEMENTATION
//#include "stb_image_write.h"

//extern std::vector<Simulation*> simulations;
//extern INT_T activeSimulation;

Objects::Objects(Camera* camera_ptr, bool& status) : textures() {// , shader(defaultVertexPath, defaultFragmentPath, status) {
	this->camera = camera_ptr;
	// shader vector is empty therefore default handle is zero
	defaultShaderHandle = shaders.size();
	//defaultShaderHandle = 1;
	// create default shader
	// this currently only works with heap allocated shaders
	// so I have to have a vector of pointers
	// right now I don't know why
	shaders.push_back(new Shader(defaultVertexPath, defaultFragmentPath, status));
	shaders.push_back(new Shader(defaultVertexPath, defaultFragmentPath, status));
	// check id shader successfully generated
	if (!status) return;

	// generate vertex array object
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
}

Objects::~Objects() {
	for (UINT_T i = 0; i < shaders.size(); ++i) {
		delete shaders[i];
	}

	for (auto it = this->begin(); it < this->end(); ++it) {
		deleteObject(*it);
	}
}


UINT_T Objects::newObject(const char* filePath, bool& status) {
	// get data from file
	UINT_T sizeVertices;
	OBJ_Data objdata;
	std::string extension = std::filesystem::path(filePath).extension().string();
	if (extension == ".obj") objdata = OBJ_GenMesh(filePath, sizeVertices, textures, status);
	//else if (extension == ".glb") objdata = GLB_GenMesh(filePath, sizeVertices, textures, status);
	else {
		std::cout << "unsupported file extension" << "\n";
		status = false;
		return 0;
	}
	// check obj file loading succedded
	if (!status) return 0;

	// find cRadius (crude calculation)
	float center[3];
	for (UINT_T i = 0; i < sizeVertices; ++i) {
		mat_add(center, &objdata.mesh[i * 8], 3, center);
	}
	mat_scalar_product(center, 1 / sizeVertices, 3, center);
	// calculate average distance to points
	for (UINT_T i = 0; i < sizeVertices; ++i) {
		float center_point_vector[3];
		mat_subtract(&objdata.mesh[i * 8], center, 3, center_point_vector);
		cRadius += vec_3_length(center_point_vector);
	}
	cRadius /= sizeVertices;

	// construct object data
	ObjectData data;
	// generate vertex buffer object
	glGenBuffers(1, &data.VBO);
	// buffer data to GPU
	glBindBuffer(GL_ARRAY_BUFFER, data.VBO);
	// pass size of mesh and position of first value
	glBufferData(GL_ARRAY_BUFFER, objdata.mesh.size() * sizeof(float), &objdata.mesh[0], GL_STATIC_DRAW);

	// transfer material data
	for (INT_T i = 0; i < objdata.materials.size(); ++i) {
		// create material data structure
		Material mat;
		// create alias for object data
		OBJ_Material& obj = objdata.materials[i];

		// transfer data
		for (INT_T j = 0; j < 3; ++j) {
			mat.ambient[j] = obj.ambient[j];
			mat.diffuse[j] = obj.diffuse[j];
			mat.specular[j] = obj.specular[j];
		}
		mat.alpha = obj.alpha;
		mat.specularExponent = obj.specularExponent;

		// create textures if applicable
		// ambient on unit 0, diffuse on unit 1, specular on uint 2
		if (obj.textureAmbient) {
			mat.textureAmbient = true;
			//mat.ambientTextureHandle = textures.newTexture(obj.ambientTexturePath.c_str(), status);
			mat.ambientTextureHandle = textures.newTextureFromBuffer(obj.ambientTexturePointer, status);
			if (!status) return 0;
		}
		if (obj.textureDiffuse) {
			mat.textureDiffuse = true;
			//mat.diffuseTextureHandle = textures.newTexture(obj.diffuseTexturePath.c_str(), status);
			mat.diffuseTextureHandle = textures.newTextureFromBuffer(obj.diffuseTexturePointer, status);
			if (!status) return 0;
		}
		if (obj.textureSpecular) {
			mat.textureSpecular = true;
			//mat.specularTextureHandle = textures.newTexture(obj.specularTexturePath.c_str(), status);
			mat.specularTextureHandle = textures.newTextureFromBuffer(obj.specularTexturePointer, status);
			if (!status) return 0;
		}

		// generate custom shaders if required
		if (obj.customShader) {
			mat.shaderHandle = shaders.size();
			bool shaderStatus;
			shaders.push_back(new Shader(obj.vertexShaderPath.c_str(), obj.fragmentShaderPath.c_str(), shaderStatus));
			if (!shaderStatus) return 0;
		}

		data.materials.push_back(mat);
	}

	// transfer material index data
	data.materialIndexes = objdata.matIndexes;

	data.nrVertices = sizeVertices;
	data.cRadius = cRadius;

	data.exists = true;

	data.scale = targetRadius / data.cRadius;

	// get object index
	UINT_T objectHandle = objects.size();
	// push object data to objects vector
	if (freeObjectIndexes.size()) {
		objects[freeObjectIndexes[0]] = data;
		objectHandle = freeObjectIndexes[0];
		freeObjectIndexes.erase(freeObjectIndexes.begin());
		freeObjectIndexes.shrink_to_fit();
	}
	else {
		objects.push_back(data);
	}

	status = true;
	return objectHandle;
}

void Objects::newObjectT(const char* filePath) {
	threadOpen = true;
	// get data from file
	//OBJ_Data objdata;
	std::string extension = std::filesystem::path(filePath).extension().string();
	if (extension == ".obj") objdata = OBJ_GenMesh(filePath, sizeVertices, textures, status);
	//else if (extension == ".glb") objdata = GLB_GenMesh(filePath, sizeVertices, textures, status);
	else {
		std::cout << "unsupported file extension" << "\n";
		status = false;
	}
	// check obj file loading succedded

	std::cout << sizeVertices << "\n";

	// find cRadius (crude calculation)
	float center[3];
	for (UINT_T i = 0; i < sizeVertices; ++i) {
		mat_add(center, &objdata.mesh[i * 8], 3, center);
	}
	mat_scalar_product(center, 1 / sizeVertices, 3, center);
	// calculate average distance to points
	for (UINT_T i = 0; i < sizeVertices; ++i) {
		float center_point_vector[3];
		mat_subtract(&objdata.mesh[i * 8], center, 3, center_point_vector);
		cRadius += vec_3_length(center_point_vector);
	}
	cRadius /= sizeVertices;



	//stbi_write_png("./assets/EarthCloudsTex.png", 4096, 3072, 3, objdata.materials[0].diffuseTexturePointer, 4 * 4096);

	//// transfer material data
	//for (INT_T i = 0; i < objdata.materials.size(); ++i) {
	//	// create alias for object data
	//	OBJ_Material& obj = objdata.materials[i];

	//	// create textures if applicable
	//	// ambient on unit 0, diffuse on unit 1, specular on uint 2
	//	if (obj.textureAmbient) {
	//		//mat.ambientTextureHandle = textures.newTexture(obj.ambientTexturePath.c_str(), status);
	//		status = textures.loadImageToBuffer(obj.ambientTexturePath.c_str(), ambientBuffer);
	//		//if (!status) return 0;
	//	}
	//	if (obj.textureDiffuse) {
	//		//mat.diffuseTextureHandle = textures.newTexture(obj.diffuseTexturePath.c_str(), status);
	//		status = textures.loadImageToBuffer(obj.diffuseTexturePath.c_str(), diffuseBuffer);
	//		//if (!status) return 0;
	//	}
	//	if (obj.textureSpecular) {
	//		//mat.specularTextureHandle = textures.newTexture(obj.specularTexturePath.c_str(), status);
	//		status = textures.loadImageToBuffer(obj.specularTexturePath.c_str(), specularBuffer);
	//		//if (!status) return 0;
	//	}
	//}

	threadDone = true;

	return;
}

void Objects::newObjectThread(const char* filePath) {
	if (!threadOpen) {
		objectThread = std::thread(&Objects::newObjectT, this, filePath);
	}
}

bool Objects::getThreadDone() {
	return threadDone;
}

bool Objects::getThreadOpen() {
	return threadOpen;
}

UINT_T Objects::joinThread(bool& status) {
	UINT_T objectHandle = 0;
	if (threadOpen) {
		threadOpen = false;
		objectThread.join();
		threadDone = false;

		// construct object data
		ObjectData data;
		// generate vertex buffer object
		glGenBuffers(1, &data.VBO);
		// buffer data to GPU
		glBindBuffer(GL_ARRAY_BUFFER, data.VBO);
		std::cout << objdata.mesh.size() << "\n";
		// pass size of mesh and position of first value
		glBufferData(GL_ARRAY_BUFFER, objdata.mesh.size() * sizeof(float), &objdata.mesh[0], GL_STATIC_DRAW);

		// transfer material data
		for (INT_T i = 0; i < objdata.materials.size(); ++i) {
			// create material data structure
			Material mat;
			// create alias for object data
			OBJ_Material& obj = objdata.materials[i];

			// transfer data
			for (INT_T j = 0; j < 3; ++j) {
				mat.ambient[j] = obj.ambient[j];
				mat.diffuse[j] = obj.diffuse[j];
				mat.specular[j] = obj.specular[j];
			}
			mat.alpha = obj.alpha;
			mat.specularExponent = obj.specularExponent;

			// create textures if applicable
			// ambient on unit 0, diffuse on unit 1, specular on uint 2
			if (obj.textureAmbient) {
				mat.textureAmbient = true;
				//mat.ambientTextureHandle = textures.newTexture(obj.ambientTexturePath.c_str(), status);
				mat.ambientTextureHandle = textures.newTextureFromBuffer(obj.ambientTexturePointer, status);
				//if (!status) return 0;
			}
			if (obj.textureDiffuse) {
				mat.textureDiffuse = true;
				//mat.diffuseTextureHandle = textures.newTexture(obj.diffuseTexturePath.c_str(), status);
				mat.diffuseTextureHandle = textures.newTextureFromBuffer(obj.diffuseTexturePointer, status);
				//printf("%s\n", status ? "success" : "fail");
				//if (!status) return 0;
			}
			if (obj.textureSpecular) {
				mat.textureSpecular = true;
				//mat.specularTextureHandle = textures.newTexture(obj.specularTexturePath.c_str(), status);
				mat.specularTextureHandle = textures.newTextureFromBuffer(obj.specularTexturePointer, status);
				//if (!status) return 0;
			}

			// generate custom shaders if required
			if (obj.customShader) {
				printf("Custom shader\n");
				mat.shaderHandle = shaders.size();
				bool shaderStatus;
				shaders.push_back(new Shader(obj.vertexShaderPath.c_str(), obj.fragmentShaderPath.c_str(), shaderStatus));
				if (!shaderStatus) return 0;
			}

			data.materials.push_back(mat);
		}

		// transfer material index data
		data.materialIndexes = objdata.matIndexes;

		data.nrVertices = sizeVertices;
		data.cRadius = cRadius;

		data.exists = true;

		data.scale = targetRadius / data.cRadius;

		// get object index
		objectHandle = objects.size();
		// push object data to objects vector
		if (freeObjectIndexes.size()) {
			objects[freeObjectIndexes[0]] = data;
			objectHandle = freeObjectIndexes[0];
			freeObjectIndexes.erase(freeObjectIndexes.begin());
			freeObjectIndexes.shrink_to_fit();
		}
		else {
			objects.push_back(data);
		}

		status = true;
	}
	//return threadObjectHandle;
	return objectHandle;
}


void Objects::deleteObject(UINT_T objectHandle) {
	ObjectData& object = objects[objectHandle];
	// deallocate vector memory
	object.materials.clear();
	object.materialIndexes.clear();
	object.materials.shrink_to_fit();
	object.materialIndexes.shrink_to_fit();

	// delete vertex buffer object
	glDeleteBuffers(1, &object.VBO);

	// delete textures
	for (auto it = object.materials.begin(); it != object.materials.end(); ++it) {
		Material& mat = *it;
		if (mat.textureAmbient) textures.deleteTexture(mat.ambientTextureHandle);
		if (mat.textureDiffuse) textures.deleteTexture(mat.diffuseTextureHandle);
		if (mat.textureSpecular) textures.deleteTexture(mat.specularTextureHandle);
	}

	object.exists = false;
	freeObjectIndexes.push_back(objectHandle);
}


void Objects::setPosition(UINT_T objectHandle, float* position) {
	for (INT_T i = 0; i < 3; ++i) {
		objects[objectHandle].position[i] = position[i];
	}
}

void Objects::setOrientation(UINT_T objectHandle, float* orientation) {
	for (INT_T i = 0; i < 3; ++i) {
		objects[objectHandle].orientation[i] = orientation[i];
	}
}

void Objects::setScale(UINT_T objectHandle, float scale) {
	objects[objectHandle].scale = scale;
}

void Objects::setName(UINT_T objectHandle, std::string name) {
	objects[objectHandle].name = name;
}

void Objects::setOpaque(UINT_T objectHandle, bool opaque) {
	objects[objectHandle].opaque = opaque;
}

void Objects::setVisible(UINT_T objectHandle, bool visible) {
	objects[objectHandle].visible = visible;
}

void Objects::setSimulated(UINT_T objectHandle, bool simulated) {
	objects[objectHandle].simulated = simulated;
}

void Objects::setInternal(UINT_T objectHandle, bool internal) {
	objects[objectHandle].internal = internal;
}

void Objects::setMass(UINT_T objectHandle, float mass) {
	objects[objectHandle].mass = mass;
}

void Objects::setVelocity(UINT_T objectHandle, float* velocity) {
	for (INT_T i = 0; i < 3; ++i) {
		objects[objectHandle].velocity[i] = velocity[i];
	}
}

void Objects::setRadius(UINT_T objectHandle, float radius) {
	objects[objectHandle].scale = radius / objects[objectHandle].cRadius;
}


void Objects::getPosition(UINT_T objectHandle, float* writeback) {
	for (INT_T i = 0; i < 3; ++i) {
		writeback[i] = objects[objectHandle].position[i];
	}
}

void Objects::getOrientation(UINT_T objectHandle, float* writeback) {
	for (INT_T i = 0; i < 3; ++i) {
		writeback[i] = objects[objectHandle].orientation[i];
	}
}

float Objects::getScale(UINT_T objectHandle) {
	return objects[objectHandle].scale;
}

UINT_T Objects::getSize(UINT_T objectHandle) {
	return objects[objectHandle].nrVertices;
}

std::string Objects::getName(UINT_T objectHandle) {
	return objects[objectHandle].name;
}

bool Objects::getOpaque(UINT_T objectHandle) {
	return objects[objectHandle].opaque;
}

bool Objects::getVisible(UINT_T objectHandle) {
	return objects[objectHandle].visible;
}

bool Objects::getSimulated(UINT_T objectHandle) {
	return objects[objectHandle].simulated;
}

bool Objects::getInternal(UINT_T objectHandle) {
	return objects[objectHandle].internal;
}

float Objects::getMass(UINT_T objectHandle) {
	return objects[objectHandle].mass;
}

void Objects::getVelocity(UINT_T objectHandle, float* writeback) {
	for (INT_T i = 0; i < 3; ++i) {
		writeback[i] = objects[objectHandle].velocity[i];
	}
}

float Objects::getRadius(UINT_T objectHandle) {
	return objects[objectHandle].cRadius * objects[objectHandle].scale;
}


void Objects::setMaterialValue(UINT_T objectHandle, UINT_T material, MaterialValue attribute, float* value) {
	Material& mat = objects[objectHandle].materials[material];

	switch (attribute) {
	case MaterialValue_ambient:
		mat_copy(value, 3, mat.ambient);
		break;
	case MaterialValue_diffuse:
		mat_copy(value, 3, mat.diffuse);
		break;
	case MaterialValue_specular:
		mat_copy(value, 3, mat.specular);
		break;
	case MaterialValue_alpha:
		mat.alpha = *value;
		break;
	case MaterialValue_specularExponent:
		mat.specularExponent = *value;
		break;
	default:
		break;
	}
}

void Objects::getMaterialValue(UINT_T objectHandle, UINT_T material, MaterialValue attribute, float* value) {
	Material& mat = objects[objectHandle].materials[material];

	switch (attribute) {
	case MaterialValue_ambient:
		mat_copy(mat.ambient, 3, value);
		break;
	case MaterialValue_diffuse:
		mat_copy(mat.diffuse, 3, value);
		break;
	case MaterialValue_specular:
		mat_copy(mat.specular, 3, value);
		break;
	case MaterialValue_alpha:
		*value = mat.alpha;
		break;
	case MaterialValue_specularExponent:
		*value = mat.specularExponent;
		break;
	default:
		break;
	}
}

UINT_T Objects::getMaterialCount(UINT_T objectHandle) {
	return objects[objectHandle].materials.size();
}


void Objects::render(UINT_T objectHandle) {
	//Simulation& simulation = *simulations[activeSimulation];
	//Simulation& simulation = *(Simulation*)simulationptr;
	// create alias for object
	ObjectData& object = objects[objectHandle];
	//Shader& shader = *shaders[defaultShaderHandle];

	//shader.use();

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::make_vec3(object.position));
	// rotate according to north east down frame euler angles
	// this will likely change later, this is just maths I already have working.
	model = glm::rotate(model, glm::radians(object.orientation[2]), glm::vec3(0.0f, -1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(object.orientation[1]), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(object.orientation[0]), glm::vec3(0.0f, 0.0f, -1.0f));
	float& scale = object.scale;
	model = glm::scale(model, glm::vec3(scale, scale, scale));

	//glm::mat4 view = simulation.camera.getViewMatrix();
	glm::mat4 view = camera->getViewMatrix();
	//glm::mat4 projection = simulation.camera.getProjectionMatrix();
	glm::mat4 projection = camera->getProjectionMatrix();

	//shader.setMat4("model", glm::value_ptr(model));
	//shader.setMat4("view", glm::value_ptr(view));
	//shader.setMat4("projection", glm::value_ptr(projection));

	//glBindBuffer(GL_ARRAY_BUFFER, object.VBO); // bind vbo of shape

	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // vertex attribute pointer
	//glEnableVertexAttribArray(0);

	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // vertex normal attribute pointer
	//glEnableVertexAttribArray(1);

	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // texture attribute pointer
	//glEnableVertexAttribArray(2);

	// create last shader varaible with effectively impossible number
	UINT_T lastShaderHandle = UINT_T_MAX;

	// load materials
	for (INT_T i = 0; i < object.materialIndexes.size(); ++i) {
		UINT_T shaderHandle = object.materials[object.materialIndexes[i].second].shaderHandle;
		Shader& shader = *shaders[shaderHandle];
		if (shaderHandle != lastShaderHandle) {
			shader.use();

			shader.setMat4("model", glm::value_ptr(model));
			shader.setMat4("view", glm::value_ptr(view));
			shader.setMat4("projection", glm::value_ptr(projection));

			// camera position can be useful
			float position[3];
			//simulation.camera.getPosition(position);
			camera->getPosition(position);
			shader.setVec3("position", position);
			// target also useful
			float target[3];
			//simulation.camera.getTarget(target);
			camera->getTarget(target);
			shader.setVec3("target", target);

			glBindBuffer(GL_ARRAY_BUFFER, object.VBO); // bind vbo of shape

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // vertex attribute pointer
			glEnableVertexAttribArray(0);

			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // vertex normal attribute pointer
			glEnableVertexAttribArray(1);

			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // texture attribute pointer
			glEnableVertexAttribArray(2);
		}
		lastShaderHandle = shaderHandle;

		// 3 because triangles
		int startVertex = object.materialIndexes[i].first;

		int nrVertices;
		if (i + 1 >= object.materialIndexes.size()) { // check if i is out of bounds
			nrVertices = object.nrVertices - object.materialIndexes[i].first;
		}
		else {
			nrVertices = object.materialIndexes[i + 1].first - object.materialIndexes[i].first;
		}


		if (object.materials[object.materialIndexes[i].second].textureDiffuse) {
			textures.bind(object.materials[object.materialIndexes[i].second].diffuseTextureHandle, 1);
			shader.setInt("texture0", 1);
			shader.setInt("textureDiffuse", 1);
		}
		else {
			shader.setVec3("color", object.materials[object.materialIndexes[i].second].diffuse);
			shader.setInt("textureDiffuse", 0);
		}

		glDrawArrays(GL_TRIANGLES, startVertex, nrVertices);
	}
}

void Objects::renderAll() {
	// render opaque object first
	for (UINT_T i = 0; i < objects.size(); ++i) {
		if (objects[i].exists && objects[i].visible && objects[i].opaque) {
			render(i);
		}
	}
	for (UINT_T i = 0; i < objects.size(); ++i) {
		if (objects[i].exists && objects[i].visible && !objects[i].opaque) {
			render(i);
		}
	}
}


// iterator operator overloading

Objects::iterator& Objects::iterator::operator++() {
	INT_T i;
	for (i = this->n + 1; i < parent->objects.size(); ++i) {
		if (parent->objects[i].exists) {
			this->n = i;
			break;
		}
	}
	// if no valid next object is found, this->n is force incremented
	// to trigger a loop break in the caller
	if (this->n != i)++this->n;
	return *this;
}

Objects::iterator Objects::iterator::operator++(int) {
	iterator temp = *this;
	INT_T i;
	for (i = this->n + 1; i < parent->objects.size(); ++i) {
		if (parent->objects[i].exists) {
			this->n = i;
			break;
		}
	}
	// if no valid next object is found, this->n is force incremented
	// to trigger a loop break in the caller
	if (this->n != i)++this->n;
	return temp;
}

bool Objects::iterator::operator==(const Objects::iterator& i2) {
	return (this->n == i2.n);
}

bool Objects::iterator::operator!=(const Objects::iterator& i2) {
	return (this->n != i2.n);
}

bool Objects::iterator::operator<(const Objects::iterator& i2) {
	return (this->n < i2.n);
}

UINT_T& Objects::iterator::operator*() {
	return this->n;
}


// iterator functions

Objects::iterator Objects::begin() {
	iterator it;
	it.parent = this;
	// find the first existing object
	for (UINT_T n = 0; n < objects.size(); ++n) {
		if (objects[n].exists) {
			it.n = n;
			break;
		}
	}
	return it;
}

Objects::iterator Objects::end() {
	iterator it;
	it.parent = this;
	//it.n = objects.size() - 1;
	for (INT_T n = objects.size() - 1; n >= 0; --n) {
		if (objects[n].exists) {
			it.n = _UINT(n) + 1;
			break;
		}
	}
	return it;
}

size_t Objects::size() {
	size_t count = 0;
	for (auto it = this->begin(); it != this->end(); ++it) ++count;
	return count;
}