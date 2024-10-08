#include "one.h"

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
        app.initApp();
        std::cerr << "one has initiated \n";
    }

    void One::loop() {
        while (!window.shouldClose()) {//closes window if close
            glfwPollEvents();
            app.drawFrame();
            
        }
        //wait until operations in command queue are finished
        vkDeviceWaitIdle(app.getLogicalDevice());
    };

    void One::end() {
        std::cerr << "one has terminated \n";
    };
}