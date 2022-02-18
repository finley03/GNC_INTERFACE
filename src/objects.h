//#ifndef OBJECTS_H
//#define OBJECTS_H
//
//#include <string>
//#include <vector>
//#include <iostream>
//
//#include <glm/glm.hpp> // gl mathematics library
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>
//
//#include "shaders.h"
//#include "obj.h"
//#include "camera.h"
//
//class Objects {
//private:
//	unsigned int VAO = 0;
//
//	const char* vertexFilePath = "./shaders/objects.vert";
//	const char* fragmentFilePath = "./shaders/objects.frag";
//
//	int shaderStatus;
//
//	Shader shader;
//
//
//	struct Data {
//		unsigned int VBO; // vertex buffer object containing mesh
//
//		std::vector<Material> materials; // vector of materials used in object
//
//		std::vector<unsigned int> materialIndex;
//
//		bool display = true;
//
//		float scale = 1; // scale of object to be drawn
//
//		float position[3] = { 0 };
//		float orientation[3] = { 0 };
//
//		unsigned int size; // number of vertices
//	};
//
//	std::vector<Data> objects; // vector of objects
//
//
//	// pointer to camera class
//	Camera* camera;
//
//
//public:
//	Objects(Camera* camera_ptr, int &success);
//
//	// returns index in vector of object
//	int newObject(std::string filePath, int& success);
//
//	void setPosition(int objectHandle, float* newPosition);
//	void setOrientation(int objectHandle, float* newOrientation);
//	void setScale(int objectHandle, float newScale);
//
//	void getPosition(int objectHandle, float* writeback);
//	void getOrientation(int objectHandle, float* writeback);
//	float getScale(int objectHandle);
//
//	void draw(int objectHandle);
//
//};
//
//
//extern Objects* objects;
//
//
//#endif


// header file for objects class
// this file handles object generation, manipulation
// and rendering

#ifndef OBJECTS_H
#define OBJECTS_H

#include "util.h"
#include "textures.h"
#include "shaders.h"
#include "camera.h"
#include "obj.h"
//#include "gltf.h"

typedef enum MaterialValue {
	MaterialValue_ambient,
	MaterialValue_diffuse,
	MaterialValue_specular,
	MaterialValue_alpha,
	MaterialValue_specularExponent
};

// extern camera pointer declaration
//extern Camera* camera;

class Objects {
private:
	// void pointer because can't circularly include simulation.h
	//void* simulationptr;

	struct Material {
		// colors for various reflection models
		float ambient[3] = { 0.1f, 0.1f, 0.1f };
		float diffuse[3] = { 0.4f, 0.4f, 0.4f };
		float specular[3] = { 0.0f, 0.0f, 0.0f };
		// opacity
		float alpha = 1.0f;
		float specularExponent = 32.0f;
		// true if texture is used for reflection component
		bool textureAmbient = false;
		bool textureDiffuse = false;
		bool textureSpecular = false;
		// handle to texture object
		uint32_t ambientTextureHandle;
		uint32_t diffuseTextureHandle;
		uint32_t specularTextureHandle;

		// handle to shader
		UINT_T shaderHandle = 0;
	};

	struct ObjectData {
		// data to describe positions of fredom for the object
		float position[3] = { 0.0f, 0.0f ,0.0f };
		float velocity[3] = { 0.0f, 0.0f, 0.0f };
		float orientation[3] = { 0.0f, 0.0f, 0.0f };
		// other parameters
		float mass = 1000.0f;
		float scale = 1.0f;

		// characteristic radius of object
		float cRadius = 1.0f;

		// true if object has no transparency
		bool opaque = true;
		// true if object should be rendered
		bool visible = true;
		// false if object is mothballed to be reallocated
		bool exists = true;
		// if true object is included in simulation
		bool simulated = true;
		// if true object is not visible or modifyable directly by user.
		bool internal = false;

		// number of vertices to render
		UINT_T nrVertices;

		// vertex buffer object for object
		uint32_t VBO;
		// vector of materials used in the object
		std::vector<Material> materials;
		// vector of material indexes (describes how materials are linked to vertices)
		std::vector<std::pair<UINT_T, UINT_T>> materialIndexes;

		// visible name of object
		std::string name;
	};

	Camera* camera;

	// path to default shaders
	const char* defaultVertexPath = "./shaders/default.vert";
	const char* defaultFragmentPath = "./shaders/default.frag";
	UINT_T defaultShaderHandle;

	// global vertex attribute object
	uint32_t VAO;

	// vector of shaders (currently only contains default shaders)
	std::vector<Shader*> shaders;
	// vector of objects
	std::vector<ObjectData> objects;
	// vector of unused object indexes
	std::vector<UINT_T> freeObjectIndexes;

	// texture handler object
	Textures textures;

	// private attributes for multithreading

	std::thread objectThread;
	bool threadOpen = false;
	bool threadDone = false;
	// stores handle to object created on thread
	UINT_T threadObjectHandle;
	// data for object loaded on thread
	OBJ_Data objdata;
	// size of object
	UINT_T sizeVertices;
	bool status;
	float cRadius;
	const float targetRadius = 1.0f;

	// pointer to texture buffers for use on thread
	uint8_t* ambientBuffer;
	uint8_t* diffuseBuffer;
	uint8_t* specularBuffer;
public:
	// constructor
	Objects(Camera* camera_ptr, bool& status);
	// destructor
	~Objects();

	// create new object from .obj file
	// return object handle
	UINT_T newObject(const char* filePath, bool& status);
	void newObjectT(const char* filePath);
	// start thread to load object
	void newObjectThread(const char* filePath);
	// check if thread has finished loading object
	bool getThreadDone();
	// check if thread is open
	bool getThreadOpen();
	// join thread
	UINT_T joinThread(bool& status);

	// removes object, deallocates memory
	void deleteObject(UINT_T objectHandle);

	// setters for various attributes
	void setPosition(UINT_T objectHandle, float* position);
	void setOrientation(UINT_T objectHandle, float* orientation);
	void setScale(UINT_T objectHandle, float scale);
	void setName(UINT_T objectHandle, std::string name);
	void setOpaque(UINT_T objectHandle, bool opaque);
	void setVisible(UINT_T objectHandle, bool visible);
	void setSimulated(UINT_T objectHandle, bool visible);
	void setInternal(UINT_T objectHandle, bool internal);
	void setMass(UINT_T objectHandle, float mass);
	void setVelocity(UINT_T objectHandle, float* velocity);
	void setRadius(UINT_T objectHandle, float radius);

	// getters for various attributes
	void getPosition(UINT_T objectHandle, float* writeback);
	void getOrientation(UINT_T objectHandle, float* writeback);
	float getScale(UINT_T objectHandle);
	UINT_T getSize(UINT_T objectHandle);
	std::string getName(UINT_T objectHandle);
	bool getOpaque(UINT_T objectHandle);
	bool getVisible(UINT_T objectHandle);
	bool getSimulated(UINT_T objectHandle);
	bool getInternal(UINT_T objectHandle);
	float getMass(UINT_T objectHandle);
	void getVelocity(UINT_T objectHandle, float* writeback);
	float getRadius(UINT_T objectHandle);

	void setMaterialValue(UINT_T objectHandle, UINT_T material, MaterialValue attribute, float* value);
	void getMaterialValue(UINT_T objectHandle, UINT_T material, MaterialValue attribute, float* value);
	UINT_T getMaterialCount(UINT_T objectHandle);

	// Procedure to render an object based on its handle
	void render(UINT_T objectHandle);
	// Procedure to render all objects
	void renderAll();

	// iterator to iterate through objects
	// forward iterator only
	class iterator {
	private:
	public:
		// pointer (this) to parent object
		Objects* parent;
		// handle to object
		UINT_T n = 0;

		// overloaded operators for forward iteration
		iterator& operator++();
		iterator operator++(int);

		// comparison operators
		bool operator==(const iterator& i2);
		bool operator!=(const iterator& i2);
		bool operator<(const iterator& i2);

		// dereference operator to get object handle
		UINT_T& operator*();
	};

	// public functions to get iterators
	iterator begin();
	iterator end();
	//externalIterator begin();
	//externalIterator begin()

	size_t size();
	//size_t externalSize();
};

#endif