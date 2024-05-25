#include "defines.cpp"

#include <iostream>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Mesh.h"
#include "Window.h"
#include "Camera.h"
#include "EventHandler.h"
#include "Graphics/Texture.h"
#include "GUI/Text.h"
#include "Graphics/MasterHandler.h"


auto main() -> int {
    if (Window::init(WIDTH, HEIGHT, "Terramine") != 0) {
        std::fprintf(stderr, "failed to initialize a window\n");
        return -1;
    }

    Events::init();
    Text::init();
    MasterHandler::init();

    glcall(glClearColor(0.61f, 0.86f, 1.0f, 1.0f));
    glcall(glEnable(GL_MULTISAMPLE));
    glcall(glEnable(GL_BLEND));
    glcall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    glcall(glLineWidth(2.0f));

    while (!Window::isClosed()) {
        if (Events::justPressed(GLFW_KEY_T)) {
            Events::toggleCursor();
        }

        if (Events::justPressed(GLFW_KEY_ESCAPE)) {
            MasterHandler::gui->current = pauseMenu;
            Events::toggleCursor();
        }

        MasterHandler::updateAll();
        MasterHandler::render();

        Window::swapBuffers();
        Events::pullEvents();
    }

    MasterHandler::terminate();
    Events::terminate();
    Window::terminate();

    return 0;
}

void GLClearError() {
    while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line) {
    while (GLenum error = glGetError()) {
        std::cout << "OpenGL error : " << error << ' ' << function << " : " << line << std::endl;
        return false;
    }
    return true;
}
