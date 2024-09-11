#include "one.h"

//std
#include <iostream>
#include <stdexcept>
#include <cstdlib>



int main() {
    one::One one;

    try {
        one.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}