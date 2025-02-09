#include "first_app.hpp"
#include <cstdlib>
#include <exception>
#include <iostream>

int main() {
    bb::FirstApp app{};

    try {
        app.run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
