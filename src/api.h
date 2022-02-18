// file that declares the class structure for the main program object

#ifndef API_H
#define API_H

#include "util.h"
#include "objects.h"
#include "camera.h"


class API {
private:
	// used to check init of object class
	bool objectStatus;

	const char* const gridPath = "./internal/grid.obj";
	UINT_T gridHandle = 0;
public:
	Camera camera;
	Objects objects;

	UINT_T mainObject;

	API(int width, int height);
	~API();

	bool createGrid();
	UINT_T getGridHandle();
};


#endif