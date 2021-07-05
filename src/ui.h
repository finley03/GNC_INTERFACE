#ifndef UI_H
#define UI_H

#include <cstdint>

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include "serial.h"
#include "screen.h"
#include "objects.h"
#include "camera.h"
#include "time.h"
#include "grid.h"

#include <SDL.h>

void UI_Run(SDL_Window* window, unsigned int &mainObject);

#endif