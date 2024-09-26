#include "one.h"

namespace one {

    void One::run() {
        initOne();
        loop();
        end();
    }

    void One::initOne() {
        app.initApp();
        pipeline.createPipeline();
        std::cerr << "one has initiated \n";
    }

    void One::loop() {
        while (!window.shouldClose()) {//closes window if close
            glfwPollEvents();
        }
    };

    void One::end() {
        
    };
}