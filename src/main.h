#ifndef MAIN_H
#define MAIN_H

//#include <iostream>
//#include <cstdlib>
//#include <cstdint>
//#include <thread>
//
//#include <Windows.h>
//
//#include <SDL.h>
//#include <glad/glad.h>
//
////#include <imgui.h>
////#include <imgui_impl_sdl.h>
////#include <imgui_impl_opengl3.h>
//
//#include "imgui.h"
//#include "imgui_impl_sdl.h"
//#include "imgui_impl_opengl3.h"

#include "util.h"
#include "serial.h"
#include "screen.h"
#include "objects.h"
#include "camera.h"
#include "ui.h"
#include "time.h"
#include "grid.h"


#define TITLE "Navigation Interface"
#define FRAMERATE 30.0
#define FRAMETIME (1000.0/FRAMERATE)

#define WIDTH 1200
#define HEIGHT 800


// create window and event types in global space
SDL_Window* window;
SDL_Renderer* renderer;
SDL_Event event;
SDL_GLContext gl_context;




int init();





#endif