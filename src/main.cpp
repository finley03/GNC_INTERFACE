#include "main.h"


int main(int argc, char* argv[]) {
	if (!init()) return -1;

	float width = WIDTH, height = HEIGHT;
	// initialize camera

	//Camera camera;
	camera.setScreen(width, height);
	float cameraPosition[] = { 0, 0, 10 };
	camera.setPosition(cameraPosition);
	camera.setRange(0.05, 1e4);
	camera.calculateProjectionMatrix();
	camera.calculateViewMatrix();


	// initialize gui

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	// (void)io;

	io.IniFilename = NULL;

	ImGui::StyleColorsDark();
	


	ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
	ImGui_ImplOpenGL3_Init();


	// initialize screen

	//bool status;
	//Screen screen(window, status);
	//if (!status) return -1;


	bool objectStatus;
	Objects lobjects(&camera, objectStatus);
	if (!objectStatus) return -1;
	objects = &lobjects;


	//// reference grid
	//Grid localgrid;
	////grid = Grid();
	//float position[] = { 0.0f, -0.3f, 0.0f };
	//localgrid.setPosition(position);
	//localgrid.setSize(1000);
	//localgrid.calculateModelMatrix();

	//grid = &localgrid;

	unsigned int gridHandle = objects->newObject("./internal/grid.obj", objectStatus);
	float gridPosition[3] = { 0.0f, -0.3f, 0.0f };
	objects->setPosition(gridHandle, gridPosition);

	// create object and set parameters

	unsigned int mainObject = objects->newObject("./assets/cube2.obj", objectStatus);
	if (!objectStatus) return -1;
	objects->setScale(mainObject, 0.1);
	float objectOrientation[] = { 0, 0, 0 };
	objects->setOrientation(mainObject, objectOrientation);
	float objectPosition[] = { 0, 0, 0 };
	objects->setPosition(mainObject, objectPosition);


	//std::cout << sizeof(CTRL_Set_Vec3) << "\n";


	bool run = true;
	while (run) {

		// handle events
		SDL_Event event;
		while (SDL_PollEvent(&event)) {

			ImGui_ImplSDL2_ProcessEvent(&event);
			switch (event.type) {
			case SDL_QUIT:
				run = false;
				break;

			case SDL_WINDOWEVENT:
				switch (event.window.event) {
				case SDL_WINDOWEVENT_SIZE_CHANGED:
					//glViewport(0, 0, event.window.data1, event.window.data2);
					screen->setViewport(event.window.data1, event.window.data2);
					camera.setScreen(event.window.data1, event.window.data2);
					camera.calculateProjectionMatrix();
					width = event.window.data1;
					height = event.window.data2;
				}
				break;

			case SDL_MOUSEMOTION:
				if (event.motion.state & SDL_BUTTON_MMASK && !io.WantCaptureMouse) {
					camera.mouseRevolve(event.motion.xrel, event.motion.yrel);
					camera.calculateViewMatrix();
				}
				if (event.motion.state & SDL_BUTTON_RMASK && !io.WantCaptureMouse) {
					camera.mouseTranslate(event.motion.xrel, event.motion.yrel);
					camera.calculateViewMatrix();
				}
				break;

			case SDL_MOUSEWHEEL:
				if (!io.WantCaptureMouse) {
					camera.mouseZoom(event.wheel.y);
					camera.calculateViewMatrix();
				}
				break;

			}

		}


		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(window);
		ImGui::NewFrame();


		screen->clear();



		UI_Run(window, mainObject);

		// render scene


		objects->renderAll();

		//objects->render(mainObject);

		//if (grid->show) grid->draw();
		

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		screen->swap();

		timer.frameDelay();


	}

	// shutdown

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


	return 0;
}


int init() {
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
		WIDTH, // window width
		HEIGHT, // window height
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


	screen->setViewport(WIDTH, HEIGHT);
	screen->setClearColor(0.06f, 0.05f, 0.11f, 1.0f);


	//glViewport(0, 0, WIDTH, HEIGHT); // set opengl viewport size
	//glClearColor(0.06, 0.05, 0.11, 1); // 0 red, 0 green, 0 blue, 1 alpha - 100% opacity

	glEnable(GL_DEPTH_TEST); // some opengl rendering settings
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glDisable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_MULTISAMPLE);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	gen_crc32_table();


	return true;
}
