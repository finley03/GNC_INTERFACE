#ifndef SCREEN_H
#define SCREEN_H

#include <iostream>
#include <glad/glad.h>
#include <SDL.h>

#include "shaders.h"

//class Screen {
//private:
//	unsigned int FBO; // framebuffer object
//	unsigned int RBO; // renderbuffer object
//	unsigned int SCRTEX; // screen texture
//
//	unsigned int VBO; // vertex buffer object
//
//	const float vertices[24] = {
//		1.0f, -1.0f, 1.0f, 0.0f,
//		-1.0f, -1.0f, 0.0f, 0.0f,
//		-1.0f, 1.0f, 0.0f, 1.0f,
//		-1.0f, 1.0f, 0.0f, 1.0f,
//		1.0f, 1.0f, 1.0f, 1.0f,
//		1.0f, -1.0f, 1.0f, 0.0f
//	};
//
//	const char* vertexFilePath = "./shaders/screen.vert";
//	const char* fragmentFilePath = "./shaders/screen.frag";
//
//	int shaderStatus;
//	Shader screenShader;
//
//	// keep pointer to window for screen swap
//	SDL_Window* window;
//
//	float clearColor[3] = { 0.1f, 0.1f, 0.1f };
//
//public:
//	Screen(unsigned int width, unsigned int height, SDL_Window* window, int& success);
//
//	void clear();
//
//	void swap();
//
//	void close();
//
//};


class Screen {
private:
	SDL_Window* window;

public:
	Screen(SDL_Window* window_ptr);

	void clear();
	void swap();
};


#endif