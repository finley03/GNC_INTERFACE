#ifndef OBJ_H
#define OBJ_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#include <sstream>

struct Material {
	float color[3] = { 1, 1, 1 };
};


std::vector<float> OBJgenMesh(std::string filePath, int& success, unsigned int& size);

std::vector<Material> OBJgetMaterialData(std::string filePath, int& success);

std::vector<unsigned int> OBJgetMaterialIndexes(std::string filePath, int& success);


#endif