#include "Window.h"

namespace one {

    Window::Window(uint32_t w, uint32_t h, std::string name):WIDTH(w), HEIGHT(h), windowName(name) {
        initWindow();
    }

    void Window::initWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); //will take out later(needs work)

        //3rd parameter allas to set a specific monitor to open window on
        window = glfwCreateWindow(WIDTH, HEIGHT, windowName.c_str(), nullptr, nullptr);

        std::cerr << "window has initiated \n";
    }

    bool Window::shouldClose() {
        return glfwWindowShouldClose(window);
    }

    void Window::createSurface(const VkInstance& instance, VkSurfaceKHR& surface) {// a surface is what connects vulkan to the window, 
        //it holds the images and the window takes car of making it for each 
        //type of system.
        if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != 
            VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }
    }

    void Window::getFramebufferSize(int& width, int& height) {
        return glfwGetFramebufferSize(window, &width, &height);
    }

    Window::~Window() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }
}