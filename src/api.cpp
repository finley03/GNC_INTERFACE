#include "api.h"

API::API(int width, int height) : camera(), objects(&this->camera, objectStatus) {
	if (!objectStatus) return;

	camera.setScreen(width, height);
	float cameraPosition[] = { 3.26f, 4.99f, 5.34f };
	camera.setPosition(cameraPosition);
	camera.setRange(0.05, 1e4);
	camera.calculateProjectionMatrix();
	camera.calculateViewMatrix();
}

API::~API() {

}

bool API::createGrid() {
	bool status;
	gridHandle = objects.newObject(gridPath, status);
	if (!status) std::cout << "Grid initialization failed\n";
	return status;
}

UINT_T API::getGridHandle() {
	return gridHandle;
}