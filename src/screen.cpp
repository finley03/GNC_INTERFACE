//#include "screen.h"
//
//
//
////Screen::Screen(unsigned int width, unsigned int height, SDL_Window* window_ptr, int& success) : screenShader(vertexFilePath, fragmentFilePath, shaderStatus) {
////    if (!shaderStatus) {
////        success = false;
////        return;
////    }
////
////    window = window_ptr;
////
////
////    glGenFramebuffers(1, &FBO);
////    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
////
////    glGenTextures(1, &SCRTEX);
////    glBindTexture(GL_TEXTURE_2D, SCRTEX);
////    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
////    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
////    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
////    glBindTexture(GL_TEXTURE_2D, 0);
////
////    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, SCRTEX, 0);
////
////    glGenRenderbuffers(1, &RBO);
////    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
////    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
////    glBindRenderbuffer(GL_RENDERBUFFER, 0);
////
////    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
////
////    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
////        std::cout << "Failed to create framebuffer" << std::endl;
////        success = false;
////        return;
////    }
////
////    glBindFramebuffer(GL_FRAMEBUFFER, 0);
////
////
////
////    glGenBuffers(1, &VBO);
////    glBindBuffer(GL_ARRAY_BUFFER, VBO);
////    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
////
////    screenShader.setInt("scrTex", 0);
////
////    success = true;
////
////}
////
////
////void Screen::clear() {
////    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
////    glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.0f);
////    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
////}
////
////
////void Screen::swap() {
////    glBindFramebuffer(GL_FRAMEBUFFER, 0);
////    glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.0f);
////    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
////
////    screenShader.use();
////
////    glActiveTexture(GL_TEXTURE0);
////    glBindTexture(GL_TEXTURE_2D, SCRTEX);
////    glBindBuffer(GL_ARRAY_BUFFER, VBO);
////
////    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
////    glEnableVertexAttribArray(0);
////
////    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
////    glEnableVertexAttribArray(1);
////
////    glDrawArrays(GL_TRIANGLES, 0, 6);
////    glBindBuffer(GL_ARRAY_BUFFER, 0);
////
////
////    //SDL_GL_SwapWindow(window);
////}
////
////
////void Screen::close() {
////    screenShader.close();
////}:
//
//
//Screen::Screen(SDL_Window* window_ptr) {
//	window = window_ptr;
//}
//
//
//void Screen::clear() {
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//}
//
//
//void Screen::swap() {
//	SDL_GL_SwapWindow(window);
//}


#include <glad/glad.h>
#include "screen.h"

constexpr uint32_t ANTIALIASING_SAMPLES = 9;


Screen::Screen(SDL_Window* window, bool& success) : screenShader(vertexFilePath, fragmentFilePath, shaderStatus) {
    // check shader compiled successfully
    if (!shaderStatus) {
        success = false;
        return;
    }

    // init window reference
    this->window = window;

    this->textureUnit = textureUnit;

    // get window size
    SDL_GetWindowSize(window, &width, &height);

    // generate framebuffer object
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    // create multisample framebuffer
    glGenTextures(1, &MSAABUFFER);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, MSAABUFFER);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, ANTIALIASING_SAMPLES, GL_RGB, width, height, GL_TRUE);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

    // assign texture as framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, MSAABUFFER, 0);


    // create renderbuffer object
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, ANTIALIASING_SAMPLES, GL_DEPTH_COMPONENT24, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // link renderbuffer to framebuffer
    //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);

    // check framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Failed to create framebuffer" << std::endl;
        success = false;
        return;
    }

    // unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // generate framebuffer object
    glGenFramebuffers(1, &IFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, IFBO);


    // create screen texture for rendering
    glGenTextures(1, &SCREENTEX);
    glBindTexture(GL_TEXTURE_2D, SCREENTEX);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // assign texture as framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, SCREENTEX, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // check framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Failed to create framebuffer" << std::endl;
        success = false;
        return;
    }

    // unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // create vertex buffer object for rendering quad
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

    // set texture uniform in shader to represent texture unit 0
    screenShader.setInt("scrTex", 0);

    success = true;
}


Screen::Screen(INT_T width, INT_T height, bool& success) : screenShader(vertexFilePath, fragmentFilePath, shaderStatus) {
    // check shader compiled successfully
    if (!shaderStatus) {
        success = false;
        return;
    }

    this->textureUnit = textureUnit;

    // generate framebuffer object
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    // create multisample framebuffer
    glGenTextures(1, &MSAABUFFER);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, MSAABUFFER);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, ANTIALIASING_SAMPLES, GL_RGB, width, height, GL_TRUE);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

    // assign texture as framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, MSAABUFFER, 0);


    // create renderbuffer object
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, ANTIALIASING_SAMPLES, GL_DEPTH_COMPONENT24, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // link renderbuffer to framebuffer
    //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);

    // check framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Failed to create framebuffer" << std::endl;
        success = false;
        return;
    }

    // unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // generate framebuffer object
    glGenFramebuffers(1, &IFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, IFBO);


    // create screen texture for rendering
    glGenTextures(1, &SCREENTEX);
    glBindTexture(GL_TEXTURE_2D, SCREENTEX);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // assign texture as framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, SCREENTEX, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // check framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Failed to create framebuffer" << std::endl;
        success = false;
        return;
    }

    // unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // create vertex buffer object for rendering quad
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

    // set texture uniform in shader to represent texture unit 0
    screenShader.setInt("scrTex", 0);

    success = true;
}


Screen::~Screen() {
    // delete all opengl buffers
    glDeleteTextures(1, &MSAABUFFER);
    glDeleteTextures(1, &SCREENTEX);
    glDeleteFramebuffers(1, &FBO);
    glDeleteFramebuffers(1, &IFBO);
    glDeleteRenderbuffers(1, &RBO);
}


void Screen::clear() {
    // assign viewport dimensions
    glViewport(0, 0, width, height);

    // attach correct framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    // set clear color
    glClearColor(clearColor.bit.r, clearColor.bit.g, clearColor.bit.b, clearColor.bit.a);
    // clear screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // enable buffer depth bit testing
    glEnable(GL_DEPTH_TEST);
}


void Screen::swap() {
    // blit multisampled framebuffer to normal colorbuffer
    glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, IFBO);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    // clear screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(clearColor.bit.r, clearColor.bit.g, clearColor.bit.b, clearColor.bit.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    // use quad shader
    screenShader.use();

    // set textures and uniforms
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, SCREENTEX);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // swap buffer to window
    SDL_GL_SwapWindow(window);
}


uint32_t Screen::getTexture() {
    // blit multisampled framebuffer to normal colorbuffer
    glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, IFBO);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    // clear screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(clearColor.bit.r, clearColor.bit.g, clearColor.bit.b, clearColor.bit.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    // use quad shader
    screenShader.use();

    // set textures and uniforms
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, SCREENTEX);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return SCREENTEX;
}


void Screen::setClearColor(float r, float g, float b, float a) {
    clearColor.bit.r = r;
    clearColor.bit.g = g;
    clearColor.bit.b = b;
    clearColor.bit.a = a;
    // function clamps values from 0 to 1
    glClearColor(r, g, b, a);
}

void Screen::setClearColor(float* color) {
    for (uint8_t i = 0; i < 4; ++i) clearColor.reg[i] = color[i];
    Color_Type& c = clearColor.bit;
    glClearColor(c.r, c.g, c.b, c.a);
}

void Screen::getClearColor(float& r, float& g, float& b, float& a) {
    Color_Type& c = clearColor.bit;
    r = c.r;
    g = c.g;
    b = c.b;
    a = c.a;
}

void Screen::getClearColor(float* color) {
    for (uint8_t i = 0; i < 4; ++i) color[i] = clearColor.reg[i];
}

void Screen::setViewport(int32_t width, int32_t height) {
    this->width = width;
    this->height = height;

    glViewport(0, 0, width, height);

    // reallocate memory for different buffers
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, MSAABUFFER);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, ANTIALIASING_SAMPLES, GL_RGB, width, height, GL_TRUE);

    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, ANTIALIASING_SAMPLES, GL_DEPTH_COMPONENT24, width, height);

    glBindTexture(GL_TEXTURE_2D, SCREENTEX);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
}