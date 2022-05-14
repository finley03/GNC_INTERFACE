#include "ui.h"
#include "screen.h"
#include "camera.h"
#include "objects.h"
#include "time.h"
#include "csys.h"
#include "api.h"
#include "serial.h"
#include "routeasm.h"

void UI_ConfigureStyle();

void UI_Settings(bool* p_open);

// forward function definitions for main menu
void UI_MenuBar(SDL_Window* window);
void UI_MainMenu(SDL_Window* window, unsigned int& mainObject, bool* p_open);
void UI_Rendering(unsigned int& mainObject);
void UI_Model(unsigned int& mainObject);
void UI_Model_Tracking(unsigned int& mainObject);
void UI_SerialPorts();
void UI_DataPolling();
void UI_Commands();
void UI_Route(SDL_Window* window);
void UI_Parameters();
void UI_EEPROM();

// for telemetry menu
void UI_TelemetryReadout(SDL_Window* window, bool* p_open);
void UI_NavDataPacket();
void UI_NavSelftestPacket();

void UI_TableFloat(const char* text, float a);
void UI_TableFloat3(const char* text, float a, float b, float c);
void UI_TableInt(const char* text, int a);
void UI_TableHex8(const char* text, uint8_t a);
void UI_TableHex16(const char* text, uint16_t a);
void UI_TableHex32(const char* text, uint32_t a);
void UI_TableTestInt(const char* text, uint8_t code);

void UI_Vec3TreeNode(const char* text, CTRL_Param parameter, float* value, bool enableWrite = false, const char* format = "%.3f");
void UI_ScalarTreeNode(const char* text, CTRL_Param parameter, float* value, bool enableWrite = false, const char* format = "%.3f");
void UI_ScalarTreeNode_LoadValue(const char* text, CTRL_Param parameter, float* value, bool enableWrite, float* valueptr, const char* buttonText, const char* format = "%.3f");
void UI_IntTreeNode(const char* text, CTRL_Param parameter, int* value, bool enableWrite);
void UI_Bool32TreeNode(const char* text, CTRL_Param parameter, int32_t* bools, int numBools, bool enableWrite, const char** boolLabels);

//bool UI_FSReadDialog(SDL_Window* window, std::string& writeback, bool* p_open, const char* extension = "", bool hideOtherExtensions = false);
bool UI_FSReadDialog(SDL_Window* window, std::string& writeback, bool* p_open, std::vector<const char*> extensions = { "" }, bool hideOtherExtensions = false);
bool UI_FSWriteDialog(SDL_Window* window, std::string& writeback, bool* p_open);

extern Csys* csys;
extern Screen* screen;
extern Timer* frameTimer;
extern API* api;

// global variables for route compilation
uint8_t* route_data = nullptr;
INT_T route_data_size = -1;
std::string routeFile;
std::string fileString;
std::filesystem::path routeFilePath;
bool binary = false;
static std::string compileLog;
bool compiled = false;

// global variables for position and orientation tracking
static int positionItem = -1;
static int orientationItem = -1;

static int flightMode = -1;
static const char* flightModeNames[] = {
	"Manual",
	"Manual heading-hold",
	"Auto waypoint"
};


void UI(SDL_Window* window) {
	// set up new frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(window);
	ImGui::NewFrame();

	UI_MenuBar(window);

	api->objects.renderAll();

	// render user interface
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


void UI_ConfigureStyle() {
	ImGuiStyle& style = ImGui::GetStyle();

	style.Colors[ImGuiCol_WindowBg] = ImVec4(15.0f / 0xff, 15.0f / 0xff, 15.0f / 0xff, 200.0f / 0xff);
}


enum Settings_Enum {
	Settings_BackgroundColor,
	Settings_Enum_Size
};

const char* const settingsText[Settings_Enum_Size] {
	"Background Color"
};


void UI_Settings(bool* p_open) {
	ImGui::Begin("Settings", p_open);
	ImGui::End();
}


void UI_MenuBar(SDL_Window* window) {
	// booleans to track state
	static bool show_main_menu = false;
	static bool show_telemetry_readout = false;
	static bool show_imgui_demo = false;
	static bool open_route = false;
	static bool show_csys_display = false;
	static bool show_settings = false;

	// imgui io handler reference
	ImGuiIO& io = ImGui::GetIO();

	// window size information
	int windowHeight, windowWidth;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);

	// remove window size constraint
	/*ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(2, 2));*/

	// set constraints on window position and size
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	// minimum window height is smaller then height of menu therefore minimum menu height applies
	ImGui::SetNextWindowSize(ImVec2(windowWidth, 10));

	bool p_open;

	// flags for window creation
	ImGuiWindowFlags windowflags =
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_MenuBar |
		ImGuiWindowFlags_NoResize;

	// remove window minimum size
	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0, 0));
	// begin menu bar
	ImGui::Begin("Menu Bar", &p_open, windowflags);

	ImGui::PopStyleVar();

	//static std::string routeFile;
	//static std::filesystem::path routeFilePath;

	if (ImGui::BeginMenuBar()) {
		ImGui::TextUnformatted("Autopilot Interface");
		if (ImGui::BeginMenu("File##menubar")) {
			ImGui::MenuItem("Open Route##menubar", NULL, &open_route);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View##menubar")) {
			ImGui::MenuItem("Main Menu##menubar", NULL, &show_main_menu);
			ImGui::MenuItem("Telemetry Readout##menubar", NULL, &show_telemetry_readout);
			ImGui::MenuItem("CSYS Display##menubar", NULL, &show_csys_display);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Tools##menubar")) {
			ImGui::MenuItem("Settings##menubar", NULL, &show_settings);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Debug##menubar")) {
			ImGui::MenuItem("ImGui Demo Window##menubar", NULL, &show_imgui_demo);
			ImGui::EndMenu();
		}

		char routeBuffer[64];
		if (routeFile.empty()) {
			sprintf(routeBuffer, "No route open");
		}
		else if (route_data_size >= 0) {
			sprintf(routeBuffer, "Loaded route: %s (%d bytes)", routeFilePath.filename().string().c_str(), route_data_size);
		}
		else {
			sprintf(routeBuffer, "Loaded route: %s", routeFilePath.filename().string().c_str());
		}
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - ImGui::CalcTextSize(routeBuffer).x - 10);
		ImGui::TextUnformatted(routeBuffer);

		ImGui::EndMenuBar();
	}

	/*if (show_main_menu) UI_MainMenu(window, mainObject, &show_main_menu);
	if (show_telemetry_readout) UI_TelemetryReadout(window, &show_telemetry_readout);
	if (show_imgui_demo) ImGui::ShowDemoWindow(&show_imgui_demo);
	if (show_csys_display) csys->showWindow();*/

	if (open_route) {
		bool fileOpened = UI_FSReadDialog(window, routeFile, &open_route, { ".bin", ".txt" }, true);
		if (fileOpened) {
			std::string extension = std::filesystem::path(routeFile).extension().string();
			if (extension == ".bin") {
				binary = true;
				compiled = true;
				readFileToByteArray(routeFile, route_data, route_data_size);
			}
			else if (extension == ".txt") {
				binary = false;
				compiled = false;
				readFileToString(routeFile, fileString);
				route_data_size = -1;
			}
			routeFilePath = std::filesystem::path(routeFile);
			compileLog.clear();
		}
	}

	ImGui::End();

	if (show_main_menu) UI_MainMenu(window, api->mainObject, &show_main_menu);
	if (show_telemetry_readout) UI_TelemetryReadout(window, &show_telemetry_readout);
	if (show_imgui_demo) ImGui::ShowDemoWindow(&show_imgui_demo);
	if (show_csys_display) csys->showWindow();
	if (show_settings) UI_Settings(&show_settings);
	UI_Model_Tracking(api->mainObject);
}


void UI_MainMenu(SDL_Window* window, unsigned int& mainObject, bool* p_open) {
	// imgui io handler reference
	ImGuiIO& io = ImGui::GetIO();

	// window size information
	int windowHeight, windowWidth;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);

	// set constraints on window position and size
	ImGui::SetNextWindowPos(ImVec2(0, 18));
	ImGui::SetNextWindowSizeConstraints(ImVec2(250, windowHeight - 18), ImVec2(FLT_MAX, windowHeight - 18));

	// flags for window creation
	ImGuiWindowFlags windowflags =
		ImGuiWindowFlags_NoMove;


	ImGui::Begin("Main Menu", p_open, windowflags);

	UI_Model(mainObject);
	UI_Rendering(mainObject);
	UI_SerialPorts();
	UI_DataPolling();
	UI_Commands();
	UI_Route(window);
	UI_Parameters();
	UI_EEPROM();

	// close poll thread
	if (serial.get_pollDone()) {
		serial.join_pollthread();
	}

	ImGui::End();

}


void UI_TelemetryReadout(SDL_Window* window, bool* p_open) {
	// imgui io handler reference
	ImGuiIO& io = ImGui::GetIO();

	// window size information
	int windowHeight, windowWidth;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);

	// set constraints on window position and size
	//ImGui::SetNextWindowPos(ImVec2(main_menu_width + 2, 20));
	ImGui::SetNextWindowSizeConstraints(ImVec2(350, windowHeight - 18), ImVec2(FLT_MAX, windowHeight - 18));

	// flags for window creation
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoMove;


	ImGui::Begin("Telemetry", p_open, window_flags);

	ImVec2 window_size = ImGui::GetWindowSize();
	ImGui::SetWindowPos(ImVec2(windowWidth - window_size.x, 18)); // windowHeight - window_size.y

	UI_NavDataPacket();
	UI_NavSelftestPacket();

	ImGui::End();
}


void UI_Rendering(unsigned int& mainObject) {
	static int trackItem = 0;

	float cameraPosition[3], cameraTarget[3], cameraFOV;
	api->camera.getPosition(cameraPosition);
	api->camera.getTarget(cameraTarget);
	cameraFOV = api->camera.getFov();

	if (trackItem == 1) {
		api->objects.getPosition(mainObject, cameraTarget);
		api->camera.setTargetTrack(cameraTarget);
		api->camera.calculateViewMatrix();
	}
	else if (trackItem == 2) {
		api->objects.getPosition(mainObject, cameraTarget);
		api->camera.setTarget(cameraTarget);
		api->camera.calculateViewMatrix();
	}



	if (!ImGui::CollapsingHeader("Rendering")) return;


	ImGui::LabelText("Function", "Value");

	ImGuiInputTextFlags inputflags =
		ImGuiInputTextFlags_EnterReturnsTrue;

	bool validInputs = true;
	bool apply = false;

	// default frame cap
	static float frameCap = frameTimer->getRateCap();
	//if (ImGui::InputScalar("Frame Cap##rendering", ImGuiDataType_Float, &frameCap, NULL, NULL, "%.1f", inputflags)) {
	//	apply = true;
	//}
	if (ImGui::InputFloat("Frame Cap##rendering", &frameCap, NULL, NULL, "%.1f", inputflags)) {
		apply = true;
	}
	if (frameCap < 0) {
		validInputs = false;
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Negative values are not valid!");
	}
	else if (frameCap < 1) {
		validInputs = false;
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Program will be unusable!");
	}

	// apply inputs if enter key is pressed and input is valid
	if (apply && validInputs) {
		frameTimer->setRateCap(frameCap);
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	static bool showGrid = true;
	if (ImGui::Checkbox("Show Grid##rendering", &showGrid)) {
		// handle of grid (should) be 0
		api->objects.setVisible(api->getGridHandle(), showGrid);
	}

	//grid->show = showGrid;
	static float gridPosition[3];
	static bool once = true;
	if (once) api->objects.getPosition(api->getGridHandle(), gridPosition), once = false;
	static float gridHeight = gridPosition[1];
	if (showGrid) {
		if (ImGui::InputFloat("Grid Height##rendering", &gridHeight, NULL, NULL, "%.1f", inputflags)) {
			gridPosition[1] = gridHeight;
			//grid->setPosition(gridPosition);
			//grid->calculateModelMatrix();
			api->objects.setPosition(api->getGridHandle(), gridPosition);
		}
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	// displays camera parameters and allows user to give definite parameters
	ImGui::Text("Camera");
	
	// camera tracking selection box
	const char* trackItems[] = { "None", "Tracking", "Target" };
	ImGui::Combo("Tracking##rendering", &trackItem, trackItems, IM_ARRAYSIZE(trackItems));

	if (ImGui::InputFloat3("Position##camera", cameraPosition, "%.2f", inputflags)) {
		api->camera.setPosition(cameraPosition);
		api->camera.calculateViewMatrix();
	}
	if (ImGui::InputFloat3("Target##camera", cameraTarget, "%.2f", inputflags)) {
		api->camera.setTarget(cameraTarget);
		api->camera.calculateViewMatrix();
	}
	if (ImGui::DragFloat("FOV##camera", &cameraFOV, 0.5f, 10.0f, 135.0f, "%.1f")) {
		api->camera.setFov(cameraFOV);
		api->camera.calculateProjectionMatrix();
	}


	ImGui::Spacing();
}


void UI_Model(unsigned int& mainObject) {
	// flags for numeric inputs
	ImGuiInputTextFlags inputflags =
		ImGuiInputTextFlags_EnterReturnsTrue;

	// return if not open
	if (ImGui::CollapsingHeader("Model")) {
	//if (ImGui::CollapsingHeader("Model")) {
		ImGui::Text("Model Parameters");
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Text("Position");

		// item position
		const char* positionItems[] = { "Manual", "Kalman Position" };
		ImGui::Combo("Source##modelposition", &positionItem, positionItems, IM_ARRAYSIZE(positionItems));

		static float threezeros[3] = { 0 };

		if (ImGui::Button("Reset Position")) {
			api->objects.setPosition(mainObject, threezeros);
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Text("Orientation");

		// item orientation
		const char* orientationItems[] = { "Manual", "Kalman Orientation", "Accel-Mag Orientation" };
		ImGui::Combo("Source##modelorientation", &orientationItem, orientationItems, IM_ARRAYSIZE(orientationItems));

		if (ImGui::Button("Reset Orientation")) {
			api->objects.setOrientation(mainObject, threezeros);
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Text("Size");

		static float scale = api->objects.getScale(mainObject);
		if (ImGui::InputFloat("Scale##model", &scale, NULL, NULL, "%.3f", inputflags)) {
			api->objects.setScale(mainObject, scale);
		}

		ImGui::Spacing();
	}


	// code that always runs
	//UI_Model_Tracking(mainObject);
}


void UI_Model_Tracking(unsigned int& mainObject) {
	// flags for numeric inputs
	ImGuiInputTextFlags inputflags =
		ImGuiInputTextFlags_EnterReturnsTrue;
	// create alias for nav_data_packet.bit
	Nav_Data_Packet_Type& data = nav_data_packet.bit;

	float modelPosition[3];
	api->objects.getPosition(mainObject, modelPosition);

	// case for "Manual" tracking
	if (positionItem == 0) {
		if (ImGui::InputFloat3("Position##model", modelPosition, "%.2f", inputflags)) {
			api->objects.setPosition(mainObject, modelPosition);
		}
	}
	// case for "Kalman Position" tracking
	if (positionItem == 1) {
		// position needs to be translated from NORTH EAST DOWN frame
		// to EAST UP SOUTH frame
		modelPosition[0] = data.position_y;
		modelPosition[1] = -data.position_z; // - 20
		modelPosition[2] = -data.position_x;
		api->objects.setPosition(mainObject, modelPosition);
	}


	float modelOrientation[3];
	api->objects.getOrientation(mainObject, modelOrientation);

	// case for "Manual" tracking
	if (orientationItem == 0) {
		if (ImGui::InputFloat3("Orientation##model", modelOrientation, "%.2f", inputflags)) {
			api->objects.setOrientation(mainObject, modelOrientation);
		}
	}
	// case for "Kalman Orientation" tracking
	else if (orientationItem == 1) {
		modelOrientation[0] = data.orientation_x;
		modelOrientation[1] = data.orientation_y;
		modelOrientation[2] = data.orientation_z;
		api->objects.setOrientation(mainObject, modelOrientation);
	}
	// case for "Accel-Mag Orientation" tracking
	else if (orientationItem == 2) {
		modelOrientation[0] = data.accelmagorientation_x;
		modelOrientation[1] = data.accelmagorientation_y;
		modelOrientation[2] = data.accelmagorientation_z;
		api->objects.setOrientation(mainObject, modelOrientation);
	}
}


void UI_SerialPorts() {
	if (!ImGui::CollapsingHeader("Serial Ports")) return;

	// Value on left, Attribute on right
	ImGui::LabelText("Attribute", "Value");

	// 32 character port name buffer
	static char portName[32];
	ImGui::InputTextWithHint("Port Name##serial", "COM1", portName, IM_ARRAYSIZE(portName));

	// default baud rate
	static uint32_t baud = 9600;
	ImGui::InputScalar("Baud Rate##serial", ImGuiDataType_U32, &baud);

	// space for buttons
	ImGui::Spacing();

	// connect attempted is true if last button pressed is "connect"
	static bool connectAttempted = false;
	// connectStatus true if connection currently established
	static bool connectStatus = false;

	if (ImGui::Button("Connect")) {
		connectAttempted = true;

		if (serial.get_status()) {
			serial.close();
		}

		//connectStatus = serial.open_port(portName, baud);
		serial.open_port(portName, baud);
	}
	if (connectAttempted) {
		ImGui::SameLine();

		if (serial.get_status()) {
			ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Successfully Connected");
		}
		else {
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Error Connecting to Port");
		}
	}

	if (ImGui::Button("Disconnect##serial")) {
		if (serial.get_status()) {
			serial.close();
		}

		connectAttempted = false;
		//connectStatus = false;
	}
	if (!serial.get_status()) {
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.0f, 0.0f, 1.0f, 1.0f), "Disconnected");
	}

	// space before next header
	ImGui::Spacing();

}


void UI_DataPolling() {
	if (!ImGui::CollapsingHeader("Data Polling")) return;


	// text input boxes should apply parameters as soon as enter key is pressed
	ImGuiInputTextFlags inputflags =
		ImGuiInputTextFlags_EnterReturnsTrue;

	{
		ImGui::Text("Nav_Data_Packet");
		ImGui::Spacing();

		// checkbox for continuous polling
		static bool continuousPoll = false;
		ImGui::Checkbox("Poll Continuously", &continuousPoll);
		ImGui::Spacing();

		if (continuousPoll) {
			bool validInputs = true;
			// true if parameters should be applied
			bool apply = false;
			// rate of continuous polling
			static float pollRate = serial.get_pollRate();
			// input poll rate
			if (ImGui::InputScalar("Poll Rate", ImGuiDataType_Float, &pollRate, NULL, NULL, NULL, inputflags)) {
				apply = true;
			}
			if (pollRate <= 0) {
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Invalid Poll Rate");
				validInputs = false;
			}
			if (apply && validInputs) {
				serial.set_pollRate(pollRate);
			}
			ImGui::Spacing();
		}

		// if continuously polling and continuous poll checkbox unchecked stop
		if (!continuousPoll && serial.get_continuousState()) {
			serial.join_pollthread();
		}

		// change button text if running continuously
		const char* pollButtonText = "Poll Data##1";
		if (continuousPoll && serial.get_continuousState()) {
			pollButtonText = "Stop Polling";
		}

		// static bool to show if button has been pressed while not connected to port
		static bool notConnectedAttempt = false;

		uint16_t pollWriteBuffer = 0x0081;

		if (ImGui::Button(pollButtonText)) {
			// run if port is open
			if (serial.get_status()) {
				notConnectedAttempt = false;
				// continuous polling
				if (continuousPoll) {
					// if currently not polling and port is open
					if (!serial.get_continuousState()) {
						serial.start_pollthread(pollWriteBuffer, nav_data_packet.reg, sizeof(nav_data_packet.reg), true);
					}
					else {
						serial.join_continuousthread();
					}
				}
				// single poll
				else {
					// start thread if not running
					if (!serial.get_pollState() && !serial.get_pollDone()) {
						serial.start_pollthread(pollWriteBuffer, nav_data_packet.reg, sizeof(nav_data_packet.reg), false);
					}
				}
			}
			else {
				notConnectedAttempt = true;
			}
		}

		// turn off notconnectedattempt if connection restablished
		if (serial.get_status()) {
			notConnectedAttempt = false;
		}

		// if port disconnects but pollthread is running close.
		if (!serial.get_status() && (serial.get_pollState() || serial.get_continuousState())) {
			serial.join_pollthread();
			serial.join_continuousthread();
			notConnectedAttempt = true;
		}

		// display warning if attempt to connect and not connected
		if (!serial.get_status() && notConnectedAttempt) {
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Not Connected to Port");
		}
	}

	ImGui::Spacing();

	// polling interface for Nav_Selftest_Packet

	ImGui::Separator();

	{
		ImGui::Text("Nav_Selftest_Packet");
		ImGui::Spacing();

		static bool notConnectedAttempt = false;
		// bool true if user attempts to poll while continuous polling operation is happening
		static bool warnOtherPollEvents = false;

		uint16_t pollWriteBuffer = 0x0080;

		if (ImGui::Button("Poll Data##2")) {
			if (serial.get_status()) {
				notConnectedAttempt = false;

				// start if thread is not running
				if (!serial.get_pollState() && !serial.get_pollDone()) {
					serial.start_pollthread(pollWriteBuffer, nav_selftest_packet.reg, sizeof(nav_selftest_packet.reg), false);
					warnOtherPollEvents = false;
				}
				else {
					warnOtherPollEvents = true;
				}
			}
			else {
				notConnectedAttempt = true;
			}
		}

		// turn of notConnectAttempt if connection reestablished
		if (serial.get_status()) {
			notConnectedAttempt = false;
		}

		// warn if attempt to connect while disconnected
		if (!serial.get_status() && notConnectedAttempt) {
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Not Connected to Port");
		}

		// turn off warnOtherPollEvents if other polling operations stop
		if (!serial.get_pollState()) {
			warnOtherPollEvents = false;
		}

		if (serial.get_pollState() && warnOtherPollEvents) {
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Cannot poll packet while other\npolling operations are in progress");
		}
	}


	ImGui::Spacing();

}


void UI_Commands() {
	if (!ImGui::CollapsingHeader("Commands")) return;

	ImGui::Text("Control Processor");
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::Combo("Flight Mode##commands", &flightMode, flightModeNames, IM_ARRAYSIZE(flightModeNames));
	char buffer[32];
	if (flightMode != -1) {
		sprintf(buffer, "apply##flightmode");
		if (ImGui::Button(buffer)) {
			serial.start_scalarsetthread(_FLIGHT_MODE, (float*)&flightMode);
		}
		ImGui::SameLine();
	}
	sprintf(buffer, "read##flightmode");
	if (ImGui::Button(buffer)) {
		serial.start_scalarreadthread(_FLIGHT_MODE, (float*)&flightMode);
	}

	if (ImGui::Button("Arm Motor")) serial.start_sendcommand(0x0005);
	if (ImGui::Button("Disarm Motor")) serial.start_sendcommand(0x0006);
	if (ImGui::Button("Start Guidance")) serial.start_sendcommand(0x0003);
	if (ImGui::Button("Stop Guidance")) serial.start_sendcommand(0x0004);
	if (ImGui::Button("Reset Guidance")) serial.start_sendcommand(0x0002);
	if (ImGui::Button("Enable NAV computer polling")) serial.start_sendcommand(0x0000);
	if (ImGui::Button("Disable NAV computer polling")) serial.start_sendcommand(0x0001);
	if (ImGui::Button("Reset Processor##control")) serial.start_sendcommand(0x007F);

	ImGui::Spacing();
	ImGui::Text("Navigation Processor");
	ImGui::Separator();
	ImGui::Spacing();

	if (ImGui::Button("Calibrate magnetometer")) serial.start_sendcommand(0x0082);
	ImGui::SameLine();
	ImGui::Text("(Disables NAV computer polling)");
	if (ImGui::Button("Calibrate gyro")) serial.start_sendcommand(0x0088);
	ImGui::SameLine();
	ImGui::Text("(Disables NAV computer polling)");
	if (ImGui::Button("Enable Kalman filter")) serial.start_sendcommand(0x0083);
	if (ImGui::Button("Disable Kalman filter")) serial.start_sendcommand(0x0084);
	if (ImGui::Button("Enable Kalman Orientation Update")) serial.start_sendcommand(0x0086);
	if (ImGui::Button("Disable Kalman Orientation Update")) serial.start_sendcommand(0x0087);
	if (ImGui::Button("Reset Processor##navigation")) serial.start_sendcommand(0x00FF);

	ImGui::Spacing();
}


void UI_Route(SDL_Window* window) {
	if (!ImGui::CollapsingHeader("Route")) return;

	static bool open_route = false;
	if (ImGui::Button("Open Route")) {
		open_route = true;
	}

	//static std::filesystem::path routeFilePath;
	//static uint8_t* fileBin = nullptr;

	if (open_route) {
		bool fileOpened = UI_FSReadDialog(window, routeFile, &open_route, { ".bin", ".txt" }, true);
		if (fileOpened) {
			std::string extension = std::filesystem::path(routeFile).extension().string();
			if (extension == ".bin") {
				binary = true;
				compiled = true;
				readFileToByteArray(routeFile, route_data, route_data_size);
			}
			else if (extension == ".txt") {
				binary = false;
				compiled = false;
				readFileToString(routeFile, fileString);
				route_data_size = -1;
			}
			routeFilePath = std::filesystem::path(routeFile);
			compileLog.clear();
		}
	}

	if (!routeFile.empty()) {
		if (binary) {
			ImGui::Text("Loaded route: %s (%d bytes)", routeFilePath.filename().string().c_str(), route_data_size);
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Warning: File is a compiled binary.\nViewing, editing, and validation\nare unsupported.");
		}
		else {
			ImGui::Text("Loaded route: %s", routeFilePath.filename().string().c_str());

			if (ImGui::Button("Compile Route")) {
				compiled = routeasm(routeFile, fileString, route_data, route_data_size);
				//for (int i = 0; i < route_data_size; ++i) printf("%02X\n", route_data[i]);
				routeasm_get_log(compileLog);
			}

			if (ImGui::BeginListBox("##routecompilelog", ImVec2(-FLT_MIN, 50))) {
				ImGui::TextWrapped(compileLog.c_str());
				ImGui::EndListBox();
			}
		}

		if (compiled) {
			static bool notConnectedAttempt = false;
			// bool true if user attempts to poll while continuous polling operation is happening
			static bool warnOtherPollEvents = false;

			static bool writeRoute = false;
			if (ImGui::Button("Write Route Data##eeprom")) {
				if (serial.get_status()) {
					notConnectedAttempt = false;
					if (!serial.get_pollState() && !serial.get_pollDone()) {
						writeRoute = true;
					}
					else {
						warnOtherPollEvents = true;
					}
				}
				else {
					notConnectedAttempt = true;
				}
			}

			if (writeRoute) {
				//for (INT_T i = 0; i <= route_data_size / 64; ++i) {
				if (!serial.get_pollState()) {
					static INT_T i = 0;
					if (i <= route_data_size / 64) {
						printf("writing %d bytes at address %04X\n", (i == route_data_size / 64) ? route_data_size % 64 : 64, i * 64);
						serial.start_eepromwritenthread(i * 64, route_data + (i * 64), (i == route_data_size / 64) ? route_data_size % 64 : 64);
						++i;
					}
					else {
						i = 0;
						writeRoute = false;
					}
				}
				//}
			}

			// turn of notConnectAttempt if connection reestablished
			if (serial.get_status()) {
				notConnectedAttempt = false;
			}

			// warn if attempt to connect while disconnected
			if (!serial.get_status() && notConnectedAttempt) {
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Not Connected to Port");
			}

			// turn off warnOtherPollEvents if other polling operations stop
			if (!serial.get_pollState()) {
				warnOtherPollEvents = false;
			}

			if (serial.get_pollState() && warnOtherPollEvents) {
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Cannot write data while other\noperations are in progress");
			}
		}
	}
}


void UI_Parameters() {
	if (!ImGui::CollapsingHeader("Parameters")) return;

	static bool enableWriting = false;
	ImGui::Checkbox("Enable Writing##parameters", &enableWriting);
	ImGui::Spacing;

	ImGui::Separator();
	ImGui::Text("CTRL Processor");
	ImGui::Spacing();

	static float pid_x_val[3];
	UI_Vec3TreeNode("PID X", _PID_X, pid_x_val, enableWriting);

	static float pid_y_val[3];
	UI_Vec3TreeNode("PID Y", _PID_Y, pid_y_val, enableWriting);

	static float pid_z_val[3];
	UI_Vec3TreeNode("PID Z", _PID_Z, pid_z_val, enableWriting);

	static float thro_config[3];
	UI_Vec3TreeNode("Thro config ZPR", _THRO_CONFIG, thro_config, enableWriting);

	static float x_mix_val[3];
	UI_Vec3TreeNode("X channel mix", _X_MIX, x_mix_val, enableWriting);

	static float y_mix_val[3];
	UI_Vec3TreeNode("Y channel mix", _Y_MIX, y_mix_val, enableWriting);

	static float z_mix_val[3];
	UI_Vec3TreeNode("Z channel mix", _Z_MIX, z_mix_val, enableWriting);

	static float position_pid[3];
	UI_Vec3TreeNode("Position PID", _POSITION_PID, position_pid, enableWriting);

	static float heading_pid[3];
	UI_Vec3TreeNode("Heading PID", _HEADING_PID, heading_pid, enableWriting);

	static float altitude_pid[3];
	UI_Vec3TreeNode("Altitude PID", _ALTITUDE_PID, altitude_pid, enableWriting);

	static float waypoint_threshold;
	UI_ScalarTreeNode("Waypoint Threshold", _WAYPOINT_THRESHOLD, &waypoint_threshold, enableWriting);

	static float channel_trim[3];
	UI_Vec3TreeNode("Channel Trim", _CHANNEL_TRIM, channel_trim, enableWriting);

	static int32_t channel_reverse;
	const char* channelReverseLabels[] = { "Aileron", "Elevator", "Rudder" };
	UI_Bool32TreeNode("Channel Reverse", _CHANNEL_REVERSE, &channel_reverse, 3, enableWriting, channelReverseLabels);

	static float angle_of_attack;
	UI_ScalarTreeNode("Angle of Attack", _AOA, &angle_of_attack, enableWriting);

	static float roll_limit;
	UI_ScalarTreeNode("Roll Limit", _ROLL_LIMIT, &roll_limit, enableWriting);

	static float pitch_limit;
	UI_ScalarTreeNode("Pitch Limit", _PITCH_LIMIT, &pitch_limit, enableWriting);

	static float elev_turn_p;
	UI_ScalarTreeNode("Elevator Turn P", _ELEVATOR_TURN_P, &elev_turn_p, enableWriting);

	UI_IntTreeNode("Flight Mode", _FLIGHT_MODE, &flightMode, enableWriting);

	static float disable_kalman_update_delay;
	UI_ScalarTreeNode("Kalman update disable time after waypoint", _DISABLE_KALMAN_UPDATE_DELAY, &disable_kalman_update_delay, enableWriting);

	static int32_t ctrl_flags_1;
	const char* ctrlFlagLabels[] = { "Disable kalman update during turns" };
	UI_Bool32TreeNode("CTRL Flags 1", _CTRL_FLAGS_1, &ctrl_flags_1, 1, enableWriting, ctrlFlagLabels);

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Text("NAV Processor");
	ImGui::Spacing();

	static float kalman_position_uncertainty[3];
	UI_Vec3TreeNode("Kalman position uncertainty", _KALMAN_POSITION_UNCERTAINTY, kalman_position_uncertainty, enableWriting);

	static float kalman_velocity_uncertainty[3];
	UI_Vec3TreeNode("Kalman velocity uncertainty", _KALMAN_VELOCITY_UNCERTAINTY, kalman_velocity_uncertainty, enableWriting);

	static float kalman_orientation_uncertainty[3];
	UI_Vec3TreeNode("Kalman orientation uncertainty", _KALMAN_ORIENTATION_UNCERTAINTY, kalman_orientation_uncertainty, enableWriting);

	static float kalman_orientation_measurement_uncertainty[3];
	UI_Vec3TreeNode("Kalman orientation measurement uncertainty", _KALMAN_ORIENTATION_MEASUREMENT_UNCERTAINTY, kalman_orientation_measurement_uncertainty, enableWriting);

	static float gnss_horizontal_uncertainty_mul;
	UI_ScalarTreeNode("GNSS horizontal uncertainty multiplier", _KALMAN_GNSS_HORIZONTAL_UNCERTAINTY_MUL, &gnss_horizontal_uncertainty_mul, enableWriting);

	static float gnss_vertical_uncertainty_mul;
	UI_ScalarTreeNode("GNSS vertical uncertainty multiplier", _KALMAN_GNSS_VERTICAL_UNCERTAINTY_MUL, &gnss_vertical_uncertainty_mul, enableWriting);

	static float kalman_baro_variance;
	UI_ScalarTreeNode("Barometer variance", _KALMAN_BARO_VARIANCE, &kalman_baro_variance, enableWriting);

	static float kalman_accel_variance;
	UI_ScalarTreeNode("Accelerometer variance", _KALMAN_ACCEL_VARIANCE, &kalman_accel_variance, enableWriting);

	static float kalman_angularvelocity_variance;
	UI_ScalarTreeNode("Angular velocity variance", _KALMAN_ANGULARVELOCITY_VARIANCE, &kalman_angularvelocity_variance, enableWriting);

	static float gnss_zerolat;
	UI_ScalarTreeNode_LoadValue("GNSS zero latitude", _KALMAN_GNSS_ZEROLAT, &gnss_zerolat, enableWriting, &(nav_data_packet.bit.latitude), "Load current latitude", "%.06f");

	static float gnss_zerolong;
	UI_ScalarTreeNode_LoadValue("GNSS zero longitude", _KALMAN_GNSS_ZEROLONG, &gnss_zerolong, enableWriting, &(nav_data_packet.bit.longitude), "Load current longitude", "%.06f");

	static float baro_height_cal;
	UI_ScalarTreeNode("Barometer height cal", _BARO_HEIGHT_CAL, &baro_height_cal, enableWriting);

	static float mag_A[9];
	UI_Vec3TreeNode("Mag Cal A-1", _MAG_A_1, mag_A);
	UI_Vec3TreeNode("Mag Cal A-2", _MAG_A_2, mag_A + 3);
	UI_Vec3TreeNode("Mag Cal A-3", _MAG_A_3, mag_A + 6);

	static float mag_b[3];
	UI_Vec3TreeNode("Mag Cal b", _MAG_B, mag_b);

	static float accel_b[3];
	UI_Vec3TreeNode("Accel Cal b", _ACCEL_B, accel_b);

	static float gyro_b[3];
	UI_Vec3TreeNode("Gyro Cal b", _GYRO_B, gyro_b);

	if (enableWriting) {
		if (ImGui::Button("Save magnetometer calibration")) serial.start_sendcommand(0x0085);
	}

	if (enableWriting) {
		if (ImGui::Button("Save gyro calibration")) serial.start_vec3savethread(_GYRO_B);
	}
}


void UI_EEPROM() {
	if (!ImGui::CollapsingHeader("EEPROM")) return;

	ImGui::Text("Read");
	ImGui::Spacing();

	static uint16_t address = 0;
	static uint8_t value = 0;
	static bool valueRead = false;

	{
		static bool notConnectedAttempt = false;
		// bool true if user attempts to poll while continuous polling operation is happening
		static bool warnOtherPollEvents = false;

		static uint16_t read_address = 0x0000;
		ImGui::InputScalar("Address (Hex)##eepromread", ImGuiDataType_U16, &read_address, NULL, NULL, "%04X", ImGuiInputTextFlags_CharsHexadecimal);

		ImGui::Spacing();

		if (ImGui::Button("Read Address##eeprom")) {
			if (serial.get_status()) {
				notConnectedAttempt = false;
				if (!serial.get_pollState() && !serial.get_pollDone()) {
					warnOtherPollEvents = false;
					serial.start_eepromreadthread(read_address, &value);
					address = read_address;
					valueRead = true;
				}
				else {
					warnOtherPollEvents = true;
				}
			}
			else {
				notConnectedAttempt = true;
			}
		}


		// turn of notConnectAttempt if connection reestablished
		if (serial.get_status()) {
			notConnectedAttempt = false;
		}

		// warn if attempt to connect while disconnected
		if (!serial.get_status() && notConnectedAttempt) {
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Not Connected to Port");
		}

		// turn off warnOtherPollEvents if other polling operations stop
		if (!serial.get_pollState()) {
			warnOtherPollEvents = false;
		}

		if (serial.get_pollState() && warnOtherPollEvents) {
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Cannot read data while other\noperations are in progress");
		}
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Text("Write");
	ImGui::Spacing();

	{
		static bool enableWriting = false;
		ImGui::Checkbox("Enable Writing##eeprom", &enableWriting);

		static bool notConnectedAttempt = false;
		// bool true if user attempts to poll while continuous polling operation is happening
		static bool warnOtherPollEvents = false;

		if (enableWriting) {
			static uint16_t write_address = 0x0000;
			static uint8_t write_value = 0xff;
			ImGui::InputScalar("Address (Hex)##eepromwrite", ImGuiDataType_U16, &write_address, NULL, NULL, "%04X", ImGuiInputTextFlags_CharsHexadecimal);
			ImGui::InputScalar("Value (Hex)##eepromwrite", ImGuiDataType_U8, &write_value, NULL, NULL, "%02X", ImGuiInputTextFlags_CharsHexadecimal);

			if (ImGui::Button("Write Address##eeprom")) {
				if (serial.get_status()) {
					notConnectedAttempt = false;
					if (!serial.get_pollState() && !serial.get_pollDone()) {
						serial.start_eepromwritethread(write_address, write_value);
						address = write_address;
						value = write_value;
						valueRead = true;
					}
					else {
						warnOtherPollEvents = true;
					}
				}
				else {
					notConnectedAttempt = true;
				}
			}

			//ImGui::Spacing();

			//static bool writeRoute = false;
			//if (ImGui::Button("Write Route Data##eeprom")) {
			//	if (serial.get_status()) {
			//		notConnectedAttempt = false;
			//		if (!serial.get_pollState() && !serial.get_pollDone()) {
			//			writeRoute = true;
			//		}
			//		else {
			//			warnOtherPollEvents = true;
			//		}
			//	}
			//	else {
			//		notConnectedAttempt = true;
			//	}
			//}

			//if (writeRoute) {
			//	//for (INT_T i = 0; i <= route_data_size / 64; ++i) {
			//	if (!serial.get_pollState()) {
			//		static INT_T i = 0;
			//		if (i <= route_data_size / 64) {
			//			printf("writing %d bytes at address %04X\n", (i == route_data_size / 64) ? route_data_size % 64 : 64, i * 64);
			//			serial.start_eepromwritenthread(i * 64, route_data + (i * 64), (i == route_data_size / 64) ? route_data_size % 64 : 64);
			//			++i;
			//		}
			//		else {
			//			i = 0;
			//			writeRoute = false;
			//		}
			//	}
			//	//}
			//}
		}

		// turn of notConnectAttempt if connection reestablished
		if (serial.get_status()) {
			notConnectedAttempt = false;
		}

		// warn if attempt to connect while disconnected
		if (!serial.get_status() && notConnectedAttempt) {
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Not Connected to Port");
		}

		// turn off warnOtherPollEvents if other polling operations stop
		if (!serial.get_pollState()) {
			warnOtherPollEvents = false;
		}

		if (serial.get_pollState() && warnOtherPollEvents) {
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Cannot write data while other\noperations are in progress");
		}
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	// flags for table generation
	static ImGuiTableFlags tableflags =
		ImGuiTableFlags_RowBg |
		ImGuiTableFlags_BordersOuter |
		ImGuiTableFlags_BordersV;

	if (ImGui::BeginTable("eepromtable1", 2, tableflags)) {
		ImGui::TableSetupColumn("Address");
		ImGui::TableSetupColumn("Value");
		ImGui::TableHeadersRow();

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("%04X", address);
		ImGui::TableSetColumnIndex(1);
		ImGui::Text("%02X", value);

		ImGui::EndTable();
	}

	ImGui::Spacing();
}


void UI_NavDataPacket() {
	if (!ImGui::CollapsingHeader("Nav_Data_Packet")) return;

	// create alias for nav_data_packet.bit
	Nav_Data_Packet_Type& data = nav_data_packet.bit;

	// flags for table generation
	static ImGuiTableFlags tableflags =
		ImGuiTableFlags_RowBg |
		ImGuiTableFlags_BordersOuter |
		ImGuiTableFlags_BordersV;

	ImGui::Spacing();
	ImGui::Text("3 Dimensional Parameters");
	ImGui::Spacing();

	if (ImGui::BeginTable("datatable1", 4, tableflags)) {
		ImGui::TableSetupColumn("Attribute");
		ImGui::TableSetupColumn("X");
		ImGui::TableSetupColumn("Y");
		ImGui::TableSetupColumn("Z");
		ImGui::TableHeadersRow();

		// values to fill table with
		UI_TableFloat3("Position", data.position_x, data.position_y, data.position_z);
		UI_TableFloat3("Velocity", data.velocity_x, data.velocity_y, data.velocity_z);
		UI_TableFloat3("Acceleration", data.accel_x, data.accel_y, data.accel_z);
		UI_TableFloat3("Raw Accel", data.accelraw_x, data.accelraw_y, data.accelraw_z);
		UI_TableFloat3("Orientation", data.orientation_x, data.orientation_y, data.orientation_z);
		UI_TableFloat3("Acc-Mag Or.", data.accelmagorientation_x, data.accelmagorientation_y, data.accelmagorientation_z);
		UI_TableFloat3("Angular Vel.", data.angularvelocity_x, data.angularvelocity_y, data.angularvelocity_z);
		UI_TableFloat3("Magnetometer", data.mag_x, data.mag_y, data.mag_z);

		ImGui::EndTable();
	}

	ImGui::Spacing();
	ImGui::Text("GNSS Data");
	ImGui::Spacing();

	if (ImGui::BeginTable("datatable2", 2, tableflags)) {
		ImGui::TableSetupColumn("Attribute");
		ImGui::TableSetupColumn("Value");
		ImGui::TableHeadersRow();

		UI_TableFloat("Latitude", data.latitude);
		UI_TableFloat("Longitude", data.longitude);
		UI_TableFloat("Height", data.gps_height);
		UI_TableFloat("Horizontal Accuracy", data.h_acc);
		UI_TableFloat("Vertical Accuracy", data.v_acc);
		UI_TableInt("Number of Satellites", data.gps_satellites);

		ImGui::EndTable();
	}

	ImGui::Spacing();
	ImGui::Text("Other Data");
	ImGui::Spacing();

	if (ImGui::BeginTable("datatable3", 2, tableflags)) {
		ImGui::TableSetupColumn("Attribute");
		ImGui::TableSetupColumn("Value");
		ImGui::TableHeadersRow();

		UI_TableFloat("Pressure", data.pressure);
		UI_TableFloat("IMU Temperature", data.imu_temperature);
		UI_TableFloat("Barometer Temperature", data.baro_temperature);

		ImGui::EndTable();
	}

	ImGui::Spacing();
	ImGui::Text("Debug");
	ImGui::Spacing();

	if (ImGui::BeginTable("datatable4", 2, tableflags)) {
		ImGui::TableSetupColumn("Attribute");
		ImGui::TableSetupColumn("Value");
		ImGui::TableHeadersRow();

		//UI_TableInt("Device ID", data.device_id);
		UI_TableFloat("Debug1", data.debug1);
		UI_TableFloat("Debug2", data.debug2);
		UI_TableHex32("Packet CRC", data.crc);

		ImGui::EndTable();
	}

	ImGui::Spacing();
}


void UI_NavSelftestPacket() {
	if (!ImGui::CollapsingHeader("Nav_Selftest_Packet")) return;

	// create alias for nav_selftest_packet.bit
	NAV_Selftest_Packet_Type& data = nav_selftest_packet.bit;

	// flags for creating table
	static ImGuiTableFlags tableflags =
		ImGuiTableFlags_RowBg |
		ImGuiTableFlags_BordersOuter |
		ImGuiTableFlags_BordersV;

	ImGui::Spacing();
	ImGui::Text("Device Tests");
	ImGui::Spacing();

	if (ImGui::BeginTable("testtable1", 2, tableflags)) {
		ImGui::TableSetupColumn("Test");
		ImGui::TableSetupColumn("Result");
		ImGui::TableHeadersRow();

		UI_TableTestInt("Navigation Computer", data.device_code);
		UI_TableTestInt("IMU", data.imu_code);
		UI_TableTestInt("Magnetometer", data.mag_code);
		UI_TableTestInt("Barometer", data.baro_code);

		ImGui::EndTable();
	}

	ImGui::Spacing();
	ImGui::Text("Debug");
	ImGui::Spacing();

	if (ImGui::BeginTable("testtable2", 2, tableflags)) {
		ImGui::TableSetupColumn("Attribute");
		ImGui::TableSetupColumn("Value");
		ImGui::TableHeadersRow();

		UI_TableHex32("Packet CRC", data.crc);

		ImGui::EndTable();
	}

	if (data.crc == NAV_SELFTEST_PACKET_CRC_OK)
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Self Test Passed");
	else if (data.crc != 0)
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Self Test Failed");

	ImGui::Spacing();
}


void UI_TableFloat(const char* text, float a) {
	ImGui::TableNextRow();
	ImGui::TableSetColumnIndex(0);
	ImGui::Text(text);
	ImGui::TableSetColumnIndex(1);
	ImGui::Text("%f", a);
}


void UI_TableFloat3(const char* text, float a, float b, float c) {
	ImGui::TableNextRow();
	ImGui::TableSetColumnIndex(0);
	ImGui::Text(text);
	ImGui::TableSetColumnIndex(1);
	ImGui::Text("%f", a);
	ImGui::TableSetColumnIndex(2);
	ImGui::Text("%f", b);
	ImGui::TableSetColumnIndex(3);
	ImGui::Text("%f", c);
}


void UI_TableInt(const char* text, int a) {
	ImGui::TableNextRow();
	ImGui::TableSetColumnIndex(0);
	ImGui::Text(text);
	ImGui::TableSetColumnIndex(1);
	ImGui::Text("%d", a);
}


void UI_TableHex8(const char* text, uint8_t a) {
	ImGui::TableNextRow();
	ImGui::TableSetColumnIndex(0);
	ImGui::Text(text);
	ImGui::TableSetColumnIndex(1);
	ImGui::Text("%x", a);
}


void UI_TableHex16(const char* text, uint16_t a) {
	ImGui::TableNextRow();
	ImGui::TableSetColumnIndex(0);
	ImGui::Text(text);
	ImGui::TableSetColumnIndex(1);
	ImGui::Text("%x", a);
}


void UI_TableHex32(const char* text, uint32_t a) {
	ImGui::TableNextRow();
	ImGui::TableSetColumnIndex(0);
	ImGui::Text(text);
	ImGui::TableSetColumnIndex(1);
	ImGui::Text("%x", a);
}


void UI_TableTestInt(const char* text, uint8_t code) {
	ImGui::TableNextRow();
	ImGui::TableSetColumnIndex(0);
	ImGui::Text(text);
	ImGui::TableSetColumnIndex(1);
	ImGui::Text((code == 0) ? "PASS" : "FAIL");
}


void UI_Vec3TreeNode(const char* text, CTRL_Param parameter, float* value, bool enableWrite, const char* format) {
	char buffer[64];
	sprintf(buffer, "%s##param", text);
	if (ImGui::TreeNode(buffer)) {
		sprintf(buffer, "value##%sparam", text);
		ImGui::InputFloat3(buffer, value);
		sprintf(buffer, "apply##%sparam", text);
		if (ImGui::Button(buffer)) {
			serial.start_vec3setthread(parameter, value);
		}
		ImGui::SameLine();
		sprintf(buffer, "read##%sparam", text);
		if (ImGui::Button(buffer)) {
			serial.start_vec3readthread(parameter, value);
		}
		if (enableWrite) {
			ImGui::SameLine();
			sprintf(buffer, "save##%sparam", text);
			if (ImGui::Button(buffer)) {
				serial.start_vec3savethread(parameter);
			}
		}

		ImGui::TreePop();
	}
}


void UI_ScalarTreeNode(const char* text, CTRL_Param parameter, float* value, bool enableWrite, const char* format) {
	char buffer[64];
	sprintf(buffer, "%s##param", text);
	if (ImGui::TreeNode(buffer)) {
		sprintf(buffer, "value##%sparam", text);
		ImGui::InputFloat(buffer, value, NULL, NULL, format);
		sprintf(buffer, "apply##%sparam", text);
		if (ImGui::Button(buffer)) {
			serial.start_scalarsetthread(parameter, value);
		}
		ImGui::SameLine();
		sprintf(buffer, "read##%sparam", text);
		if (ImGui::Button(buffer)) {
			serial.start_scalarreadthread(parameter, value);
		}
		if (enableWrite) {
			ImGui::SameLine();
			sprintf(buffer, "save##%sparam", text);
			if (ImGui::Button(buffer)) {
				serial.start_scalarsavethread(parameter);
			}
		}

		ImGui::TreePop();
	}
}


void UI_ScalarTreeNode_LoadValue(const char* text, CTRL_Param parameter, float* value, bool enableWrite, float* valueptr, const char* buttonText, const char* format) {
	char buffer[64];
	sprintf(buffer, "%s##param", text);
	if (ImGui::TreeNode(buffer)) {
		sprintf(buffer, "value##%sparam", text);
		ImGui::InputFloat(buffer, value, NULL, NULL, format);
		sprintf(buffer, "apply##%sparam", text);
		if (ImGui::Button(buffer)) {
			serial.start_scalarsetthread(parameter, value);
		}
		ImGui::SameLine();
		sprintf(buffer, "read##%sparam", text);
		if (ImGui::Button(buffer)) {
			serial.start_scalarreadthread(parameter, value);
		}
		if (enableWrite) {
			ImGui::SameLine();
			sprintf(buffer, "save##%sparam", text);
			if (ImGui::Button(buffer)) {
				serial.start_scalarsavethread(parameter);
			}
		}
		sprintf(buffer, "%s##%sparam", buttonText, text);
		if (ImGui::Button(buffer)) {
			*value = *valueptr;
		}

		ImGui::TreePop();
	}
}


void UI_IntTreeNode(const char* text, CTRL_Param parameter, int* value, bool enableWrite) {
	char buffer[64];
	sprintf(buffer, "%s##param", text);
	if (ImGui::TreeNode(buffer)) {
		sprintf(buffer, "value##%sparam", text);
		ImGui::InputInt(buffer, value, NULL, NULL);
		sprintf(buffer, "apply##%sparam", text);
		if (ImGui::Button(buffer)) {
			serial.start_scalarsetthread(parameter, (float*)value);
		}
		ImGui::SameLine();
		sprintf(buffer, "read##%sparam", text);
		if (ImGui::Button(buffer)) {
			serial.start_scalarreadthread(parameter, (float*)value);
		}
		if (enableWrite) {
			ImGui::SameLine();
			sprintf(buffer, "save##%sparam", text);
			if (ImGui::Button(buffer)) {
				serial.start_scalarsavethread(parameter);
			}
		}

		ImGui::TreePop();
	}
}


void UI_Bool32TreeNode(const char* text, CTRL_Param parameter, int32_t* bools, int numBools, bool enableWrite, const char** boolLabels) {
	char buffer[64];
	sprintf(buffer, "%s##param", text);
	if (ImGui::TreeNode(buffer)) {
		for (int i = 0; i < numBools; ++i) {
			bool val = (bool)(*bools & (1 << i));
			if (ImGui::Checkbox(boolLabels[i], &val)) {
				if (val) {
					*bools |= (1 << i);
				}
				else {
					*bools &= ~(1 << i);
				}
			}
		}
		sprintf(buffer, "apply##%sparam", text);
		if (ImGui::Button(buffer)) {
			serial.start_scalarsetthread(parameter, (float*)bools);
		}
		ImGui::SameLine();
		sprintf(buffer, "read##%sparam", text);
		if (ImGui::Button(buffer)) {
			serial.start_scalarreadthread(parameter, (float*)bools);
		}
		if (enableWrite) {
			ImGui::SameLine();
			sprintf(buffer, "save##%sparam", text);
			if (ImGui::Button(buffer)) {
				serial.start_scalarsavethread(parameter);
			}
		}

		ImGui::TreePop();
	}
}


// procedure creates a file dialog and returns a file path
bool UI_FSReadDialog(SDL_Window* window, std::string& writeback, bool* p_open, std::vector<const char*> extensions, bool hideOtherExtensions) {
	// get imgui io handler reference (static - only needs to be initialised once)
	static ImGuiIO& io = ImGui::GetIO();
	// get window size
	INT_T windowWidth, windowHeight;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);
	// set constraints on window size and position
	ImGui::SetNextWindowPos(ImVec2((windowWidth - 500) / 2, (windowHeight - 350) / 2), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(500, 350), ImGuiCond_Once);
	// flags for window
	static ImGuiWindowFlags windowflags =
		ImGuiWindowFlags_NoCollapse;

	// create namespace declaration to reduce code
	namespace fs = std::filesystem;

	// wether the file has been opened
	bool fileOpened = false;
	// boolean that tracks whether opening unselected item has been attempted
	static bool openAttempt = false;
	// static bool that tracks wether extension is valid
	static bool invalidExtension = false;

	ImGui::Begin("Open##fs", p_open, windowflags);

	// get current path
	static fs::path path = fs::current_path();
	static std::string pathstr = path.string();

	// index of item selected
	static INT_T selected = -1;

	// button for moving to parent directory
	if (ImGui::Button("<##fs", ImVec2(25, 0))) {
		path = path.parent_path();
		pathstr = path.string();
		//std::cout << pathstr << "\n";
		// reset selected index
		selected = -1;
	}

	ImGui::SameLine();
	// show current directory
	ImGui::TextColored(ImVec4(0.8f, 0.8f, 1.0f, 1.0f), pathstr.c_str());

	float textHeight = ImGui::GetTextLineHeight();

	// seperator before directory contents
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	static ImGuiSelectableFlags selectflags =
		ImGuiSelectableFlags_AllowDoubleClick;

	// list directory contents
	INT_T index = 0;
	static fs::path selectedpath;
	// contain within list box
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
	if (ImGui::BeginListBox("##directoryfs", ImVec2(-FLT_MIN, ImGui::GetWindowHeight() - 72 - 2 * textHeight))) {
		// iterate through items in directory
		for (const auto& p : fs::directory_iterator(path)) {
			// use one color if item is directory
			if (fs::is_directory(p)) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 1.0f, 1.0f));
			// if not directory and not selected extension and hideOtherExtensions skip item
			//else if (hideOtherExtensions && strcmp(extension, p.path().extension().string().c_str()) != 0) continue;
			else if (hideOtherExtensions) {
				bool continuebool = true;
				for (auto it = extensions.begin(); it != extensions.end(); ++it) {
					if (strcmp(*it, p.path().extension().string().c_str()) == 0) {
						continuebool = false;
						break;
					}
				}
				if (continuebool) continue;
				else ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.9f, 0.9f, 1.0f));
			}
			// if file use different color
			else ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.9f, 0.9f, 1.0f));
			// create selectable element for directory element
			if (ImGui::Selectable(p.path().string().erase(0, pathstr.length()).c_str(), selected == index, selectflags)) {
				// check if left mouse button clicked twice
				if (ImGui::IsMouseDoubleClicked(0)) {
					// check if path is directory
					if (fs::is_directory(p)) {
						path = p.path();
						pathstr = path.string();
					}
					else {
						// if file extension is required and extension does not match
						bool extensionMatch = false;
						for (auto it = extensions.begin(); it != extensions.end(); ++it) {
							if (strcmp(*it, p.path().extension().string().c_str()) == 0) {
								extensionMatch = true;
								break;
							}
						}
						//if (*extension && strcmp(extension, p.path().extension().string().c_str()) != 0) {
						if (!extensionMatch) {
							invalidExtension = true;
						}
						else {
							invalidExtension = false;
							writeback = p.path().string();
							// replace backslashes with forward slashes
							// it appears std::ifstream doesn't like them
							std::replace(writeback.begin(), writeback.end(), '\\', '/');
							// close window
							*p_open = false;
							fileOpened = true;
						}

					}
					// reset selected
					selected = -1;
				}
				else {
					// select item
					selected = index;
					openAttempt = false;
					if (!fs::is_directory(p)) {
						selectedpath = p.path();
					}
				}
			}
			++index;
			ImGui::PopStyleColor();
		}
		ImGui::EndListBox();
	}
	ImGui::PopStyleColor();

	// spacing before final buttons
	ImGui::Spacing();

	// display warning message to the user if applicable
	// uses else if to only display one message at a time
	if (openAttempt) {
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Please select a file");
		ImGui::SameLine();
	}
	else if (invalidExtension) {
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Extension invalid");
		ImGui::SameLine();
	}

	// position cursor to right for buttons
	ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 140);

	// button to open file
	if (ImGui::Button("Open##fs", ImVec2(60, 0))) {
		if (selectedpath.empty()) {
			openAttempt = true;
		}
		else {
			// if file extension is required
			bool extensionMatch = false;
			for (auto it = extensions.begin(); it != extensions.end(); ++it) {
				if (strcmp(*it, selectedpath.extension().string().c_str()) == 0) {
					extensionMatch = true;
					break;
				}
			}
			//if (*extension && strcmp(extension, selectedpath.extension().string().c_str()) != 0) {
			if (!extensionMatch) {
				//if (strcmp(extension, selectedpath.extension().string().c_str()) != 0) {
				invalidExtension = true;
			}
			else {
				invalidExtension = false;
				writeback = selectedpath.string();
				std::replace(writeback.begin(), writeback.end(), '\\', '/');
				*p_open = false;
				fileOpened = true;
			}
		}
	}

	// button to cancel opening file
	ImGui::SameLine();
	if (ImGui::Button("Cancel##fs", ImVec2(60, 0))) {
		*p_open = false;
		openAttempt = false;
	}

	ImGui::End();

	// if window should close reset some parameters
	if (!*p_open) {
		selectedpath.clear();
		selected = -1;
	}

	// return true if file found
	return fileOpened;
}


// procedure creates a file dialog and returns a file path
bool UI_FSWriteDialog(SDL_Window* window, std::string& writeback, bool* p_open) {
	// get imgui io handler reference (static - only needs to be initialised once)
	static ImGuiIO& io = ImGui::GetIO();
	// get window size
	INT_T windowWidth, windowHeight;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);
	// set constraints on window size and position
	ImGui::SetNextWindowPos(ImVec2((windowWidth - 500) / 2, (windowHeight - 350) / 2), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(500, 350), ImGuiCond_Once);
	// flags for window
	static ImGuiWindowFlags windowflags =
		ImGuiWindowFlags_NoCollapse;

	// create namespace declaration to reduce code
	namespace fs = std::filesystem;

	// whether the file has been opened
	bool fileSaved = false;
	// boolean that tracks whether opening unselected item has been attempted
	static bool saveAttempt = false;

	ImGui::Begin("Save As##fs", p_open, windowflags);

	// get current path
	static fs::path path = fs::current_path();
	static std::string pathstr = path.string();

	// index of item selected
	static INT_T selected = -1;

	// button for moving to parent directory
	if (ImGui::Button("<##fs", ImVec2(25, 0))) {
		path = path.parent_path();
		pathstr = path.string();
		//std::cout << pathstr << "\n";
		// reset selected index
		selected = -1;
	}

	ImGui::SameLine();
	// show current directory
	ImGui::TextColored(ImVec4(0.8f, 0.8f, 1.0f, 1.0f), pathstr.c_str());

	float textHeight = ImGui::GetTextLineHeight();

	// seperator before directory contents
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	static ImGuiSelectableFlags selectflags =
		ImGuiSelectableFlags_AllowDoubleClick;

	// list directory contents
	INT_T index = 0;
	//static fs::path selectedpath;
	// contain within list box
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
	if (ImGui::BeginListBox("##directoryfs", ImVec2(-FLT_MIN, ImGui::GetWindowHeight() - 72 - 2 * textHeight))) {
		// iterate through items in directory
		for (const auto& p : fs::directory_iterator(path)) {
			// use one color if item is directory
			if (fs::is_directory(p)) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 1.0f, 1.0f));
			// if file use different color
			else ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.9f, 0.9f, 1.0f));
			// create selectable element for directory element
			if (ImGui::Selectable(p.path().string().erase(0, pathstr.length()).c_str(), selected == index, selectflags)) {
				// check if left mouse button clicked twice
				if (ImGui::IsMouseDoubleClicked(0)) {
					// check if path is directory
					if (fs::is_directory(p)) {
						path = p.path();
						pathstr = path.string();
					}
					// reset selected
					selected = -1;
				}
				else {
					// select item
					selected = index;
					saveAttempt = false;
				}
			}
			++index;
			ImGui::PopStyleColor();
		}
		ImGui::EndListBox();
	}
	ImGui::PopStyleColor();

	// spacing before final buttons
	ImGui::Spacing();

	// display warning message to the user if applicable
	// uses else if to only display one message at a time
	if (saveAttempt) {
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Enter File Name");
		ImGui::SameLine();
	}

	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 145 - ImGui::GetCursorPosX());

	static ImGuiInputTextFlags inputflags =
		ImGuiInputTextFlags_EnterReturnsTrue;

	static char buffer[260];

	//ImGui::InputTextWithHint("##inputfs", "File Name", buffer);

	if (ImGui::InputTextWithHint("##inputfs", "File Name", buffer, 260, inputflags)) {
		if (strlen(buffer) != 0) {
			//selectedpath = path;
			//selectedpath.replace_filename(buffer);
			writeback = path.string();
			writeback.append("/");
			writeback.append(buffer);
			std::replace(writeback.begin(), writeback.end(), '\\', '/');
			fileSaved = true;
			*p_open = false;
		}
		else {
			saveAttempt = true;
		}
	}

	ImGui::SameLine();

	//// position cursor to right for buttons
	//ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 140);

	// button to save file
	if (ImGui::Button("Save##fs", ImVec2(60, 0))) {
		if (strlen(buffer) != 0) {
			//selectedpath = path;
			//selectedpath.replace_filename(buffer);
			writeback = path.string();
			writeback.append("/");
			writeback.append(buffer);
			std::replace(writeback.begin(), writeback.end(), '\\', '/');
			fileSaved = true;
			*p_open = false;
		}
		else {
			saveAttempt = true;
		}
	}

	// button to cancel saving file
	ImGui::SameLine();
	if (ImGui::Button("Cancel##fs", ImVec2(60, 0))) {
		*p_open = false;
		saveAttempt = false;
	}

	ImGui::End();

	// if window should close reset some parameters
	if (!*p_open) {
		selected = -1;
	}

	// return true if file found
	return fileSaved;
}