#include "bb_window.hpp"
#include <stdexcept>

namespace bb {
BbWindow::BbWindow(int w, int h, std::string name) : width{w}, height{h}, windowName{name} {
    initWindow();
}

BbWindow::~BbWindow() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void BbWindow::initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

void BbWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface) {
    if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface");
    }
}

void BbWindow::framebufferResizeCallback(GLFWwindow *window, int width, int height) {
    auto bbWindow = reinterpret_cast<BbWindow *>(glfwGetWindowUserPointer(window));
    bbWindow->framebufferResized = true;
    bbWindow->width = width;
    bbWindow->height = height;
}

} // namespace bb
