//#ifndef OBJ_H
//#define OBJ_H
//
//#include <string>
//#include <vector>
//#include <iostream>
//#include <fstream>
//#include <map>
//#include <sstream>
//
//struct Material {
//	float color[3] = { 1, 1, 1 };
//};
//
//
//std::vector<float> OBJgenMesh(std::string filePath, int& success, unsigned int& size);
//
//std::vector<Material> OBJgetMaterialData(std::string filePath, int& success);
//
//std::vector<unsigned int> OBJgetMaterialIndexes(std::string filePath, int& success);
//
//
//#endif


// header file for .obj file interpreter
// this largely uses C where appropriate
// for speed reasons, however C++ containers
// are sometimes used to avoid the pitfalls
// of static types

#ifndef OBJ_H
#define OBJ_H

#include "util.h"
#include "textures.h"

// Function to load an object mesh from file
// Mesh format:
// float position[3]
// float normalVector[3]
// float textureCoords[2]
OBJ_Data OBJ_GenMesh(const char* filePath, UINT_T& size, Textures& textures, bool& status);

#endif