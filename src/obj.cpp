#include "obj.h"


std::vector<float> OBJgenMesh(std::string filePath, int& success, unsigned int& size) {
	std::ifstream f(filePath); // generates file "f" from the filepath
    // check if file is open, if not, throw error
    if (!f.is_open()) {
        std::cout << "Error: Failed to find file \"" << filePath << "\"" << std::endl;
        success = false; // sets success variable to false to check later
    }
    else {
        std::cout << "Loading objects from file " << "\"" << filePath << "\"" << std::endl;
    }


    std::vector<float> vertices; // define data types to add to
    std::vector<float> vertexNormals;
    std::vector<float> textureCoords;
    std::vector<int> vertexIndices;
    std::vector<int> normalIndices;
    std::vector<int> textureIndices;

    std::string line;
    //const char* file = ;
    //f >> file[0];
    //char* line;
    //char* next_token;

    //line = strtok_s(file, "\n", &next_token); // divide by lines

    while (getline(f, line)) { // iterate over lines in file
    //while (line != nullptr) {
        if (line.empty()) continue;
        //if (line[0] == '\0') continue; // if char* is blank continue

        std::stringstream ss(line); // create stringstream from line data
        std::vector<std::string> linevec; // create string vector
        std::string tmp; // create temporary string for getline function
        while (std::getline(ss, tmp, ' ')) { // iterate over line, using getline with ' ' delimiter
            linevec.push_back(tmp); // add tmp to vector
        }

        if (linevec[0] == "v") { // case for vertex
            vertices.push_back(std::stof(linevec[1])); // convert string to float and push to vertex vector
            vertices.push_back(std::stof(linevec[2]));
            vertices.push_back(std::stof(linevec[3]));
            continue;
        }
        else if (linevec[0] == "vn") { // case for vertex normals
            vertexNormals.push_back(std::stof(linevec[1]));
            vertexNormals.push_back(std::stof(linevec[2]));
            vertexNormals.push_back(std::stof(linevec[3]));
            continue;
        }
        else if (linevec[0] == "vt") { // case for texture coordinates
            textureCoords.push_back(std::stof(linevec[1]));
            textureCoords.push_back(std::stof(linevec[2]));
            continue;
        }
        else if (linevec[0] == "f") { // case for TRIANGULAR face elements
            std::string triangle[3];
            for (unsigned int i = 1; i < linevec.size() - 2; ++i) { // creates number of shapes: n = sides - 2
                triangle[0] = linevec[1];
                triangle[1] = linevec[i + 1];
                triangle[2] = linevec[i + 2];

                //bool noTexCoord; // set bool for no texture coordinate to not send invalid data to index stream

                for (unsigned int i = 0; i < 3; ++i) { // loop through indices of triangle
                    std::stringstream indexData(triangle[i]); // generate stringstream of current working data
                    //std::vector<int> indexvec; // create vector of index data format [v, vt, vn]
                    int indexvec[3] = { 1, 1, 1 };
                    int j = 0;
                    std::string tmp;
                    while (std::getline(indexData, tmp, '/')) {
                        if (tmp != "") {
                            //indexvec.push_back(std::stoi(tmp)); // push indices to vector of index data
                            indexvec[j] = std::stoi(tmp);
                        }
                        //else {
                        //    //indexvec.push_back(1); // I would push back 0, however, 1 is subtracted from the value later and indexing an array with -1 is invalid
                        //    indexvec[j] = 1;
                        //}

                        ++j;
                    }

                    vertexIndices.push_back(indexvec[0] - 1); // add indexes to main data structures
                    textureIndices.push_back(indexvec[1] - 1); // subtract 1 because .obj is 1 indexed
                    normalIndices.push_back(indexvec[2] - 1);
                }

            }
            continue;
        }
        else if (linevec[0] == "#") { // case for comment
            continue;
        }
        else if (linevec[0] == "mtllib") { // case for material properties file
            continue;
        }
        else if (linevec[0] == "usemtl") { // case for use material declaration
            continue;
        }
        else if (linevec[0] == "o") { // case for object name
            std::cout << "Loading object " << "\"" << linevec[1] << "\"" << " from file " << "\"" << filePath << "\"" << std::endl;
            continue;
        }
        else if (linevec[0] == "g") { // case for group name
            continue;
        }
        else if (linevec[0] == "s") { // case for smooth shading
            continue;
        }
        else {
            std::cout << "File \"" << filePath << "\" contains unknown parameter \"" << linevec[0] << "\"" << std::endl;
        }


        //std::stringstream ss(line); // create stringstream from line data
        //std::vector<std::string> linevec; // create string vector
        //std::string tmp; // create temporary string for getline function
        //while (std::getline(ss, tmp, ' ')) { // iterate over line, using getline with ' ' delimiter
        //    linevec.push_back(tmp); // add tmp to vector
        //}


  
    }

    // Now main data structures have been populated from the file, the file is no longer needed

    f.close();

    // now the function will arrange the data into one std::vector, suitable for packing into
    // a single vertex buffer object.
    // the format given does not include indices, so it creates a format suitable for glDrawArrays

    std::vector<float> data; // final data structure

    for (unsigned int i = 0; i < vertexIndices.size(); ++i) { // loops through data, +=3 because there are
        // 3 vertex coordinates per vertex data element

        data.push_back(vertices[vertexIndices[i] * 3]); // add 3 vertex coordinates
        data.push_back(vertices[vertexIndices[i] * 3 + 1]);
        data.push_back(vertices[vertexIndices[i] * 3 + 2]);

        data.push_back(vertexNormals[normalIndices[i] * 3]); // add 3 verted normal values
        data.push_back(vertexNormals[normalIndices[i] * 3 + 1]);
        data.push_back(vertexNormals[normalIndices[i] * 3 + 2]);

        data.push_back(textureCoords[textureIndices[i] * 2]); // add 2 texture coordinates
        data.push_back(textureCoords[textureIndices[i] * 2 + 1]);

    }

    size = vertexIndices.size();
    std::cout << "Size: " << size << " vertices" << std::endl;

    success = true;

    return data;
}


std::string getFileFolderPath(std::string filePath) {
    return filePath.substr(0, filePath.find_last_of("/") + 1);
}



std::string getMTLfile(std::string filePath, int& success) {
    success = false;

    std::ifstream f(filePath); // generates file "f" from the filepath
    // check if file is open, if not, throw error
    if (!f.is_open()) {
        std::cout << "Error: Failed to find file to find mtl \"" << filePath << "\"" << std::endl;
        success = false; // sets success variable to false to check later
    }

    std::string line;

    while (std::getline(f, line)) { // iterate over lines in file
        if (line.empty()) continue;

        if (line.rfind("mtllib ") == 0) {
            success = true;
            f.close();
            int strPos = line.find(" ") + 1;
            return line.substr(strPos, line.npos);

        }
        else if (line.find("usemtl ", 0) == 0) {
            success = false;
            f.close();
            int strPos = line.find(" ") + 1;
            std::cout << "Couldn't find file for material \"" << line.substr(strPos, line.npos) << "\"" << std::endl;
        }

    }

    f.close();
    success = false;

    std::cout << "No mtl file found, using default parameters" << std::endl;
    return "";
}



std::vector<Material> OBJgetMaterialData(std::string filePath, int& success) {
    int mtlsuccess;
    std::string mtl = getMTLfile(filePath, mtlsuccess);
    if (!success) {
        // use default values
    }


    std::vector<Material> materials;

    std::string path = getFileFolderPath(filePath);

    std::string mtlFilePath = path + mtl;


    std::ifstream m(mtlFilePath); // generates mtl file "m" from the filepath
    // check if file is open, if not, throw error
    if (!m.is_open()) {
        std::cout << "Error: Failed to find mtl file for data \"" << mtlFilePath << "\"" << std::endl;
        success = false; // sets success variable to false to check later
        materials.push_back(Material());
        return materials;
    }

    int materialIndex = -1; // material index starts at -1 because it is incremented before the first element is added

    std::string line;

    while (std::getline(m, line)) { // iterate over lines in file
        if (line.empty()) continue;

        std::stringstream ss(line); // create stringstream from line data
        std::vector<std::string> linevec; // create string vector
        std::string tmp; // create temporary string for getline function
        while (std::getline(ss, tmp, ' ')) { // iterate over line, using getline with ' ' delimiter
            linevec.push_back(tmp); // add tmp to vector
        }
        if (linevec[0] == "newmtl") {
            ++materialIndex;
            materials.push_back(Material());
            continue;
        }
        else if (linevec[0] == "Kd") {
            materials[materialIndex].color[0] = std::stof(linevec[1]);
            materials[materialIndex].color[1] = std::stof(linevec[2]);
            materials[materialIndex].color[2] = std::stof(linevec[3]);
            continue;
        }
        else if (linevec[0] == "#") {
            continue;
        }
        else {
            continue;
        }
    }

    m.close();


    return materials;
}



std::vector<unsigned int> OBJgetMaterialIndexes(std::string filePath, int& success) {
    int mtlsuccess;
    std::string mtl = getMTLfile(filePath, mtlsuccess);
    if (!success) {
        // use default values
    }

    std::string path = getFileFolderPath(filePath);

    std::string mtlFilePath = path + mtl;

    //std::vector<std::string> materials;
    std::map<std::string, unsigned int> materials;

    std::ifstream m(mtlFilePath); // generates mtl file "m" from the filepath
    // check if file is open, if not, throw error
    if (!m.is_open()) {
        std::cout << "Error: Failed to find mtl file \"" << mtlFilePath << "\"" << std::endl;
        success = false; // sets success variable to false to check later
    }


    // add all material names in order to std::vector

    std::string line;

    while (std::getline(m, line)) { // iterate over lines in file
        if (line.empty()) continue;

        if (line.find("newmtl", 0) == 0) {
            int nameLoc = line.find(" ") + 1;
            materials[line.substr(nameLoc, line.npos)] = materials.size();
        }
    }

    m.close();


    std::ifstream f(filePath); // generates file "f" from the filepath
    // check if file is open, if not, throw error
    if (!f.is_open()) {
        std::cout << "Error: Failed to find obj file \"" << filePath << "\"" << std::endl;
        success = false; // sets success variable to false to check later
    }

    std::vector<unsigned int> indexes; // material index array, format <start index, material index>

    unsigned int faceIndex = 0;

    while (std::getline(f, line)) { // iterate over lines in file
        if (line.empty()) continue;

        if (line[0] == 'f') {
            faceIndex += (std::count(line.begin(), line.end(), ' ') - 2);
        }
        else if (line.find("usemtl", 0) == 0) {
            indexes.push_back(faceIndex); // push starting index
            int nameLoc = line.find(" ") + 1;
            indexes.push_back(materials[line.substr(nameLoc, line.npos)]);
        }
    }

    f.close();


    return indexes;
}