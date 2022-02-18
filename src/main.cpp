#include "main.h"
#include "util.h"
#include "serial.h"
#include "screen.h"
#include "objects.h"
#include "camera.h"
#include "ui.h"
#include "time.h"
#include "grid.h"
#include "csys.h"
#include "api.h"

#define TITLE "Autopilot Interface"
uint16_t width = 1200;
uint16_t height = 800;

// global variables

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Event event;
SDL_GLContext gl_context;

Screen* screen;
Csys* csys;
Timer* frameTimer;
API* api;

bool handleEvents();
bool init();
void shutdown();


int main(int argc, char* argv[]) {
	if (!init()) return -1;
	// initialize camera

	////Camera camera;
	//camera.setScreen(width, height);
	//float cameraPosition[] = { 3.26f, 4.99f, 5.34f };
	//camera.setPosition(cameraPosition);
	//camera.setRange(0.05, 1e4);
	//camera.calculateProjectionMatrix();
	//camera.calculateViewMatrix();

	// init csys display

	//csys = new Csys(&api->camera, 100, 100);

	// initialize gui

	//IMGUI_CHECKVERSION();
	//ImGui::CreateContext();
	//ImGuiIO& io = ImGui::GetIO();
	//io.IniFilename = NULL; // disable imgui ini

	//ImGui::StyleColorsDark();
	//


	//ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
	//ImGui_ImplOpenGL3_Init();

	//bool objectStatus;
	//Objects lobjects(&camera, objectStatus);
	//if (!objectStatus) return -1;
	//objects = &lobjects;

	//unsigned int gridHandle = objects->newObject("./internal/grid.obj", objectStatus);
	//float gridPosition[3] = { 0.0f, -0.3f, 0.0f };
	//objects->setPosition(gridHandle, gridPosition);
	//objects->setOpaque(gridHandle, false);

	//// create object and set parameters

	//unsigned int mainObject = objects->newObject("./assets/cube2.obj", objectStatus);
	//if (!objectStatus) return -1;
	//objects->setScale(mainObject, 0.3);
	//float objectOrientation[] = { 0, 0, 0 };
	//objects->setOrientation(mainObject, objectOrientation);
	//float objectPosition[] = { 0, 0, 0 };
	//objects->setPosition(mainObject, objectPosition);


	//bool run = true;
	//while (run) {

	//	// handle events
	//	SDL_Event event;
	//	while (SDL_PollEvent(&event)) {

	//		ImGui_ImplSDL2_ProcessEvent(&event);
	//		switch (event.type) {
	//		case SDL_QUIT:
	//			run = false;
	//			break;

	//		case SDL_WINDOWEVENT:
	//			switch (event.window.event) {
	//			case SDL_WINDOWEVENT_SIZE_CHANGED:
	//				//glViewport(0, 0, event.window.data1, event.window.data2);
	//				screen->setViewport(event.window.data1, event.window.data2);
	//				camera.setScreen(event.window.data1, event.window.data2);
	//				camera.calculateProjectionMatrix();
	//				width = event.window.data1;
	//				height = event.window.data2;
	//			}
	//			break;

	//		case SDL_MOUSEMOTION:
	//			if (event.motion.state & SDL_BUTTON_MMASK && !io.WantCaptureMouse) {
	//				camera.mouseRevolve(event.motion.xrel, event.motion.yrel);
	//				camera.calculateViewMatrix();
	//			}
	//			if (event.motion.state & SDL_BUTTON_RMASK && !io.WantCaptureMouse) {
	//				camera.mouseTranslate(event.motion.xrel, event.motion.yrel);
	//				camera.calculateViewMatrix();
	//			}
	//			break;

	//		case SDL_MOUSEWHEEL:
	//			if (!io.WantCaptureMouse) {
	//				camera.mouseZoom(event.wheel.y);
	//				camera.calculateViewMatrix();
	//			}
	//			break;

	//		}

	//	}


	//	ImGui_ImplOpenGL3_NewFrame();
	//	ImGui_ImplSDL2_NewFrame(window);
	//	ImGui::NewFrame();

	//	csys->draw();

	//	screen->clear();



	//	UI_Run(window, mainObject);

	//	// render scene


	//	objects->renderAll();

	//	//objects->render(mainObject);

	//	//if (grid->show) grid->draw();
	//	

	//	ImGui::Render();
	//	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	//	screen->swap();

	//	timer.frameDelay();


	//}

	bool run = true;
	while (run) {
		run = handleEvents();
		
		csys->draw();
		
		screen->clear();
		
		UI(window);
		
		screen->swap();
		
		frameTimer->delay();
	}

	// shutdown

	//// close poll thread
	//serial.join_continuousthread();
	//serial.join_pollthread();
	//// close serial link
	//serial.close();

	//// shutdown ImGui
	//ImGui_ImplOpenGL3_Shutdown();
	//ImGui_ImplSDL2_Shutdown();
	//ImGui::DestroyContext();

	//// close OpenGL and SDL
	//SDL_GL_DeleteContext(gl_context);
	//SDL_DestroyWindow(window);
	//SDL_DestroyRenderer(renderer);
	//SDL_Quit();

	shutdown();

	return 0;
}


// This function will handle all inputs
// and OS requests. It will return true if the
// program should continue running
bool handleEvents() {
	bool run = true;

	static ImGuiIO& io = ImGui::GetIO();

	// handle OS events
	while (SDL_PollEvent(&event)) {
		// let ImGui process events for UI
		ImGui_ImplSDL2_ProcessEvent(&event);

		switch (event.type) {

			// case for program close
		case SDL_QUIT:
			run = false;
			break;

			// case for a window change
		case SDL_WINDOWEVENT:
			switch (event.window.event) {
				// case for window resize
			case SDL_WINDOWEVENT_SIZE_CHANGED:
				screen->setViewport(event.window.data1, event.window.data2);
				api->camera.setScreen(event.window.data1, event.window.data2);
				api->camera.calculateProjectionMatrix();
				width = event.window.data1;
				height = event.window.data2;
			}
			break;

			// case for a mouse motion event
		case SDL_MOUSEMOTION:
			// case for middle mouse button (revolve)
			// only revolve if mouse is not over GUI window
			if (event.motion.state & SDL_BUTTON_MMASK && !io.WantCaptureMouse) {
				api->camera.mouseRevolve(event.motion.xrel, event.motion.yrel);
				api->camera.calculateViewMatrix();
			}
			// case for right mouse button (translate on plane)
			if (event.motion.state & SDL_BUTTON_RMASK && !io.WantCaptureMouse) {
				api->camera.mouseTranslate(event.motion.xrel, event.motion.yrel);
				api->camera.calculateViewMatrix();
			}
			break;

			// case for mouse wheel event
		case SDL_MOUSEWHEEL:
			// check GUI window is not focused
			if (!io.WantCaptureMouse) {
				api->camera.mouseZoom(event.wheel.y);
				api->camera.calculateViewMatrix();
			}
			break;
		}
	}

	return run;
}


bool init() {
	// initialize sdl video
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cout << "SDL failed to initalise video: " << SDL_GetError() << std::endl;
		return false;
	}

	// enable antialisaing
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);

	// create the window
	window = SDL_CreateWindow(
		TITLE, // window title
		SDL_WINDOWPOS_CENTERED, // x position
		SDL_WINDOWPOS_CENTERED, // y position
		width, // window width
		height, // window height
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE // window creation flags
	);

	// check window has been created
	// i.e check window is not nullptr
	if (!window) {
		std::cout << "SDL failed to create window: " << SDL_GetError() << std::endl;
		return false;
	}

	// create hardware renderer
	renderer = SDL_CreateRenderer(
		window, // reference to window object
		-1, // default to initialize first possible index
		SDL_RENDERER_ACCELERATED
	);

	// check if rendrerer is nullptr
	if (!renderer) {
		std::cout << "SDL failed to create renderer: " << SDL_GetError() << std::endl;
		return false;
	}

	// use sdl to initialize opengl context
	gl_context = SDL_GL_CreateContext(window);


	// load opengl extensions using GLAD
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		std::cout << "Failed to load OpenGL extensions" << std::endl;
		return false;
	}


	bool status;
	// initialize screen buffers
	screen = new Screen(window, status);
	// check successful init
	if (!status) return false;

	frameTimer = new Timer();

	api = new API(width, height);

	csys = new Csys(&api->camera, 100, 100);

	screen->setViewport(width, height);
	screen->setClearColor(0.06f, 0.05f, 0.11f, 1.0f);

	glEnable(GL_DEPTH_TEST); // some opengl rendering settings
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glDisable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_MULTISAMPLE);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = NULL; // disable imgui ini

	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
	ImGui_ImplOpenGL3_Init();

	UI_ConfigureStyle();

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	gen_crc32_table();


	//bool objectStatus;
	//Objects lobjects(&camera, objectStatus);
	//if (!objectStatus) return -1;
	//objects = &lobjects;

	//unsigned int gridHandle = objects->newObject("./internal/grid.obj", status);
	//float gridPosition[3] = { 0.0f, -0.3f, 0.0f };
	//objects->setPosition(gridHandle, gridPosition);
	//objects->setOpaque(gridHandle, false);

	//// create object and set parameters

	//unsigned int mainObject = objects->newObject("./assets/cube2.obj", objectStatus);
	//if (!objectStatus) return -1;
	//objects->setScale(mainObject, 0.3);
	//float objectOrientation[] = { 0, 0, 0 };
	//objects->setOrientation(mainObject, objectOrientation);
	//float objectPosition[] = { 0, 0, 0 };
	//objects->setPosition(mainObject, objectPosition);

	UINT_T mainObject = api->objects.newObject("./assets/cube2.obj", status);
	if (!status) std::cout << "Initialization failed\n";
	/*UINT_T gridHandle = api->objects.newObject("./internal/grid.obj", status);
	if (!status) std::cout << "Initialization failed\n";*/
	api->createGrid();
	UINT_T gridHandle = api->getGridHandle();

	api->objects.setScale(mainObject, 0.3);
	float gridPosition[3] = { 0.0f, -0.3f, 0.0f };
	api->objects.setPosition(gridHandle, gridPosition);
	api->objects.setOpaque(gridHandle, false);

	return true;
}


void shutdown() {
	// close poll thread
	serial.join_continuousthread();
	serial.join_pollthread();
	// close serial link
	serial.close();

	// shutdown ImGui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	// close OpenGL and SDL
	SDL_GL_DeleteContext(gl_context);
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
}