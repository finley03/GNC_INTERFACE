#include "ui.h"


// forward function definitions for main menu
void UI_MainMenu(SDL_Window* window, unsigned int& mainObject, bool* p_open);
void UI_Rendering(unsigned int& mainObject);
void UI_Model(unsigned int& mainObject);
void UI_SerialPorts();
void UI_DataPolling();
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

void UI_CTRLVec3TreeNode(const char* text, CTRL_Param parameter, float* value);


void UI_Run(SDL_Window* window, unsigned int& mainObject) {
	// booleans to track state
	static bool show_main_menu = false;
	static bool show_telemetry_readout = false;
	static bool show_imgui_demo = false;


	if (show_main_menu) UI_MainMenu(window, mainObject, &show_main_menu);
	if (show_telemetry_readout) UI_TelemetryReadout(window, &show_telemetry_readout);
	if (show_imgui_demo) ImGui::ShowDemoWindow(&show_imgui_demo);


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

	bool randomBool;

	// remove window minimum size
	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0, 0));
	// begin menu bar
	ImGui::Begin("Menu Bar", &p_open, windowflags);

	ImGui::PopStyleVar();


	if (ImGui::BeginMenuBar()) {
		ImGui::Text("Autopilot Interface");
		if (ImGui::BeginMenu("File")) {
			ImGui::MenuItem("New", NULL, &randomBool);
			ImGui::MenuItem("Open", NULL, &randomBool);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View")) {
			ImGui::MenuItem("Main Menu", NULL, &show_main_menu);
			ImGui::MenuItem("Telemetry Readout", NULL, &show_telemetry_readout);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Debug")) {
			ImGui::MenuItem("ImGui Demo Window", NULL, &show_imgui_demo);
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
	ImGui::End();

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
	if (!ImGui::CollapsingHeader("Rendering")) return;

	ImGui::LabelText("Function", "Value");

	ImGuiInputTextFlags inputflags =
		ImGuiInputTextFlags_EnterReturnsTrue;

	bool validInputs = true;
	bool apply = false;

	// default frame cap
	static float frameCap = timer.getFrameCap();
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
		timer.setFrameCap(frameCap);
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	static bool showGrid = true;
	ImGui::Checkbox("Show Grid##rendering", &showGrid);

	grid->show = showGrid;
	static float gridHeight = 0;
	if (showGrid) {
		if (ImGui::InputFloat("Grid Height##rendering", &gridHeight, NULL, NULL, "%.1f", inputflags)) {
			float gridPosition[3] = { 0, gridHeight, 0 };
			grid->setPosition(gridPosition);
			grid->calculateModelMatrix();
		}
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	// displays camera parameters and allows user to give definite parameters
	ImGui::Text("Camera");
	
	// camera tracking selection box
	const char* trackItems[] = { "None", "Tracking", "Target" };
	static int trackItem = 0;
	ImGui::Combo("Tracking##rendering", &trackItem, trackItems, IM_ARRAYSIZE(trackItems));

	float cameraPosition[3], cameraTarget[3], cameraFOV;
	camera.getPosition(cameraPosition);
	camera.getTarget(cameraTarget);
	cameraFOV = camera.getFov();

	if (ImGui::InputFloat3("Position##camera", cameraPosition, "%.2f", inputflags)) {
		camera.setPosition(cameraPosition);
		camera.calculateViewMatrix();
	}
	if (ImGui::InputFloat3("Target##camera", cameraTarget, "%.2f", inputflags)) {
		camera.setTarget(cameraTarget);
		camera.calculateViewMatrix();
	}
	if (ImGui::DragFloat("FOV##camera", &cameraFOV, 0.5f, 10.0f, 135.0f, "%.1f")) {
		camera.setFov(cameraFOV);
		camera.calculateProjectionMatrix();
	}


	if (trackItem == 1) {
		objects->getPosition(mainObject, cameraTarget);
		camera.setTargetTrack(cameraTarget);
		camera.calculateViewMatrix();
	}
	else if (trackItem == 2) {
		objects->getPosition(mainObject, cameraTarget);
		camera.setTarget(cameraTarget);
		camera.calculateViewMatrix();
	}


	ImGui::Spacing();
}


void UI_Model(unsigned int& mainObject) {
	if (!ImGui::CollapsingHeader("Model")) return;

	// flags for numeric inputs
	ImGuiInputTextFlags inputflags =
		ImGuiInputTextFlags_EnterReturnsTrue;

	// create alias for nav_data_packet.bit
	Nav_Data_Packet_Type& data = nav_data_packet.bit;

	static float threezeros[3] = { 0 };


	ImGui::Text("Model Parameters");
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::Text("Position");

	// item position
	const char* positionItems[] = { "Manual", "Kalman Position" };
	static int positionItem = -1;
	ImGui::Combo("Source##modelposition", &positionItem, positionItems, IM_ARRAYSIZE(positionItems));

	float modelPosition[3];
	objects->getPosition(mainObject, modelPosition);

	// case for "Manual" tracking
	if (positionItem == 0) {
		if (ImGui::InputFloat3("Position##model", modelPosition, "%.2f", inputflags)) {
			objects->setPosition(mainObject, modelPosition);
		}
	}
	// case for "Kalman Position" tracking
	if (positionItem == 1) {
		// position needs to be translated from NORTH EAST DOWN frame
		// to EAST UP SOUTH frame
		modelPosition[0] = data.position_y;
		modelPosition[1] = -data.position_z; // - 20
		modelPosition[2] = -data.position_x;
		objects->setPosition(mainObject, modelPosition);
	}

	if (ImGui::Button("Reset Position")) {
		objects->setPosition(mainObject, threezeros);
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::Text("Orientation");

	// item orientation
	const char* orientationItems[] = { "Manual", "Kalman Orientation", "Accel-Mag Orientation" };
	static int orientationItem = -1;
	ImGui::Combo("Source##modelorientation", &orientationItem, orientationItems, IM_ARRAYSIZE(orientationItems));

	float modelOrientation[3];
	objects->getOrientation(mainObject, modelOrientation);

	// case for "Manual" tracking
	if (orientationItem == 0) {
		if (ImGui::InputFloat3("Orientation##model", modelOrientation, "%.2f", inputflags)) {
			objects->setOrientation(mainObject, modelOrientation);
		}
	}
	// case for "Kalman Orientation" tracking
	else if (orientationItem == 1) {
		modelOrientation[0] = data.orientation_x;
		modelOrientation[1] = data.orientation_y;
		modelOrientation[2] = data.orientation_z;
		objects->setOrientation(mainObject, modelOrientation);
	}
	// case for "Accel-Mag Orientation" tracking
	else if (orientationItem == 2) {
		modelOrientation[0] = data.accelmagorientation_x;
		modelOrientation[1] = data.accelmagorientation_y;
		modelOrientation[2] = data.accelmagorientation_z;
		objects->setOrientation(mainObject, modelOrientation);
	}

	if (ImGui::Button("Reset Orientation")) {
		objects->setOrientation(mainObject, threezeros);
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::Text("Size");

	static float scale = objects->getScale(mainObject);
	if (ImGui::InputFloat("Scale##model", &scale, NULL, NULL, "%.3f", inputflags)) {
		objects->setScale(mainObject, scale);
	}

	ImGui::Spacing();
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

		//// close single poll thread
		//if (serial.get_pollDone()) {
		//	serial.join_pollthread();
		//}
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


void UI_Parameters() {
	if (!ImGui::CollapsingHeader("Parameters")) return;

	ImGui::Text("CTRL Processor");
	ImGui::Spacing();

	static float pid_x_val[3];
	UI_CTRLVec3TreeNode("PID_X", _PID_X, pid_x_val);

	static float pid_y_val[3];
	UI_CTRLVec3TreeNode("PID_Y", _PID_Y, pid_y_val);

	static float pid_z_val[3];
	UI_CTRLVec3TreeNode("PID_Z", _PID_Z, pid_z_val);
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


void UI_CTRLVec3TreeNode(const char* text, CTRL_Param parameter, float* value) {
	char buffer[32];
	sprintf(buffer, "%s##param", text);
	if (ImGui::TreeNode(buffer)) {
		sprintf(buffer, "value##%sparam", text);
		ImGui::InputFloat3(buffer, value);
		sprintf(buffer, "apply##%sparam", text);
		if (ImGui::Button(buffer)) {
			serial.start_vec3setthread(parameter, value);
		}

		ImGui::TreePop();
	}
}