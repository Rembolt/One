#include "one.h"

namespace one {

    void One::run() {
        initOne();
        loop();
        end();
    }

    

    void One::initOne() {


    }

    void One::loop() {
        while (!window.shouldClose()) {//closes window if close
            glfwPollEvents();
        }
    };

    void One::end() {

    };
}