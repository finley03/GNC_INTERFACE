#include "screen.h"



//Screen::Screen(unsigned int width, unsigned int height, SDL_Window* window_ptr, int& success) : screenShader(vertexFilePath, fragmentFilePath, shaderStatus) {
//    if (!shaderStatus) {
//        success = false;
//        return;
//    }
//
//    window = window_ptr;
//
//
//    glGenFramebuffers(1, &FBO);
//    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
//
//    glGenTextures(1, &SCRTEX);
//    glBindTexture(GL_TEXTURE_2D, SCRTEX);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glBindTexture(GL_TEXTURE_2D, 0);
//
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, SCRTEX, 0);
//
//    glGenRenderbuffers(1, &RBO);
//    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
//    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
//    glBindRenderbuffer(GL_RENDERBUFFER, 0);
//
//    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
//
//    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
//        std::cout << "Failed to create framebuffer" << std::endl;
//        success = false;
//        return;
//    }
//
//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//
//
//    glGenBuffers(1, &VBO);
//    glBindBuffer(GL_ARRAY_BUFFER, VBO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
//
//    screenShader.setInt("scrTex", 0);
//
//    success = true;
//
//}
//
//
//void Screen::clear() {
//    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
//    glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.0f);
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//}
//
//
//void Screen::swap() {
//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//    glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.0f);
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//    screenShader.use();
//
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, SCRTEX);
//    glBindBuffer(GL_ARRAY_BUFFER, VBO);
//
//    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
//    glEnableVertexAttribArray(0);
//
//    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
//    glEnableVertexAttribArray(1);
//
//    glDrawArrays(GL_TRIANGLES, 0, 6);
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//
//    //SDL_GL_SwapWindow(window);
//}
//
//
//void Screen::close() {
//    screenShader.close();
//}:


Screen::Screen(SDL_Window* window_ptr) {
	window = window_ptr;
}


void Screen::clear() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void Screen::swap() {
	SDL_GL_SwapWindow(window);
}