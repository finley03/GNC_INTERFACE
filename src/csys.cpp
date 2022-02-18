// source file for coordinate system class

#include "csys.h"
extern "C" {
#include "mat.h"
}


Csys::Csys(Camera* camera, INT_T width, INT_T height) :
	// initialize classes
	shader(csysVert, csysFrag, shaderStatus),
	screen(width, height, screenStatus) {

	this->width = width;
	this->height = height;

	// assign camera pointer
	this->camera = camera;

	// generate VBO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 42 * sizeof(float), vertices, GL_STATIC_DRAW);

	//projection = glm::ortho(-5.0f, (float)width / 5, -5.0f, (float)height / 5, 0.1f, 30.0f);
	// orthographic projection matrix
	projection = glm::ortho(-1.5f, 1.5f, 1.5f, -1.5f, 0.1f, 2.0f);

	screen.setViewport(width, height);
	screen.setClearColor(0.1f, 0.1f, 0.1f, 0.0f);
}


void Csys::showWindow() {
	// get imgui io handler reference (static - only needs to be initialised once)
	static ImGuiIO& io = ImGui::GetIO();
	// set constraints on window size
	ImGui::SetNextWindowSize(ImVec2(width + 4, width + 8), ImGuiCond_Once);
	// flags for window
	static ImGuiWindowFlags windowflags =
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoScrollbar;

	bool p_open = true;
	// begin window
	ImGui::Begin("Image Test", &p_open, windowflags);

	ImGui::SetCursorPos(ImVec2(0, 4));

	ImGui::Image((void*)texHandle, ImVec2(width, height));

	// get matrices
	glm::mat4 view = getViewMatrix();
	glm::mat4 projection = getProjectionMatrix();
	glm::mat4 mat = projection * view;
	glm::vec2 Xpos = glm::vec2(mat * glm::vec4(1.15f, 0.0f, 0.0f, 1.0f));
	glm::vec2 Ypos = glm::vec2(mat * glm::vec4(0.0f, 1.15f, 0.0f, 1.0f));
	glm::vec2 Zpos = glm::vec2(mat * glm::vec4(0.0f, 0.0f, 1.15f, 1.0f));

	float width_2 = (float)width / 2;
	float height_2 = (float)height / 2;

	//ImDrawList* drawlist = ImGui::GetWindowDrawList();
	//const char* X = "X";
	//drawlist->AddText(ImVec2(20, 20), 0xFFFFFFFF, X, X);
	ImGui::SetCursorPos(ImVec2((Xpos.x + 1) * width_2 - 3, (Xpos.y + 1) * height_2 - 3));
	ImGui::TextUnformatted("X");
	ImGui::SetCursorPos(ImVec2((Ypos.x + 1) * width_2 - 3, (Ypos.y + 1) * height_2 - 3));
	ImGui::TextUnformatted("Y");
	ImGui::SetCursorPos(ImVec2((Zpos.x + 1) * width_2 - 3, (Zpos.y + 1) * height_2 - 3));
	ImGui::TextUnformatted("Z");

	// end window
	ImGui::End();
}


void Csys::draw() {
	screen.clear();

	// use shader stored in CSYS class
	shader.use();

	// pull data from MAIN camera class
	float position[3], target[3], up[2];
	float viewPosition[3], viewTarget[3] = {0, 0, 0};
	camera->getPosition(position);
	camera->getTarget(target);
	camera->getUp(up);

	// calculate position of camera
	mat_subtract(position, target, 3, viewPosition);

	// check camera is not at the origin
	if (vec_3_length(viewPosition) == 0) return;

	// calculate unit vector
	vec_3_normalize(viewPosition, viewPosition);

	// calculate view matrix
	view = glm::lookAt(glm::make_vec3(viewPosition), glm::make_vec3(viewTarget), glm::vec3(glm::make_vec2(up), 0.0f));

	// set matrices for GPU
	shader.setMat4("view", glm::value_ptr(view));
	shader.setMat4("projection", glm::value_ptr(projection));

	// bind correct vertex buffer object
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// create vertex attribute pointer to position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// create vertex attribute pointer to color
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glDrawArrays(GL_LINES, 0, 6);

	// store texture handle for later
	texHandle = screen.getTexture();
}


uint32_t Csys::getTexHandle() {
	//return screen.getTexture();
	return texHandle;
}


glm::mat4 Csys::getViewMatrix() {
	return view;
}

glm::mat4 Csys::getProjectionMatrix() {
	return projection;
}