// header file for texture handling class

#ifndef TEXTURES_H
#define TEXTURES_H

#include "util.h"

class Textures {
private:
	// private function for returning the correct OpenGL mask
	// for the given colorspace
	bool colorSpace(int nrChannels, uint32_t& cSpaceMask);

	// private attributes for exchange between
	// multithreading functions
	int texWidth, texHeight, nrChannels;
	uint32_t cSpace;
public:
	// Function to generate new texture
	// must pass texture unit to bind texture to
	uint32_t newTexture(const char* filePath, bool& status);

	// Image loading function for use with multithreading
	// can be called from any thread
	bool loadImageToBuffer(const char* filePath, uint8_t*& buffer);

	// Image loading function for file already loaded into ram
	bool parseImageToBuffer(uint8_t* data, INT_T size, uint8_t*& buffer);

	// Texture generation function for use with multithreading
	// must be called from primary OpenGL thread
	uint32_t newTextureFromBuffer(uint8_t*& buffer, bool& status);

	// bind texture to texture unit
	void bind(uint32_t textureHandle, uint32_t textureUnit);

	// delete a texture
	void deleteTexture(uint32_t textureHandle);
};

#endif