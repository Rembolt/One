#include "window.h"

namespace one {

    Window::Window(uint32_t w, uint32_t h, std::string name):WIDTH(w), HEIGHT(h), windowName(name) {
        initWindow();
    }

    void Window::initWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); //will take out later(needs work)

        window = glfwCreateWindow(WIDTH, HEIGHT, windowName.c_str(), nullptr, nullptr);//3rd parameter allas to set a specific monitor to open window on
    }

    bool Window::shouldClose() {
        return glfwWindowShouldClose(window);
    }

    Window::~Window() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }
}