#include "One.h"

namespace one {

    void One::run() {
        initOne();
        loop();
        end();
    }

    void One::initOne() {
        //I probably will set App as a handler of all Vulkan Classes/objects 
        //and separate the bellow functions to other files as well as put everyting in a folder
        //so app can create and get set some items while runtime functions can be sent to each class
        //
        pWindow = new Window(WIDTH, HEIGHT, "One");
        pApp = new App(pWindow);
        std::cerr << "one has initiated \n";
    }

    void One::loop() {
        while (!pWindow->shouldClose()) {//closes window if close
            glfwPollEvents();
            pApp->drawFrame();
            
        }
        //wait until operations in command queue are finished
        vkDeviceWaitIdle(pApp->getDevice());
    };

    void One::end() {
        std::cerr << "one has terminated \n";
    };
}