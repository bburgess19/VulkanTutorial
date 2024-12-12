#pragma once

#include <string>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace bb {

class BbWindow {
public:
  BbWindow(int w, int h, std::string name);
  ~BbWindow();

  BbWindow(const BbWindow &) = delete;
  BbWindow &operator=(const BbWindow &) = delete;

  bool shouldClose() { return glfwWindowShouldClose(window); }
  VkExtent2D getExtent() { return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)}; }
  bool wasWindowResized() { return framebufferResized; }
  void resetWindowResizedFlag() { framebufferResized = false; }
  GLFWwindow *getGLFWwindow() const { return window; }

  void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

private:
  static void framebufferResizeCallback(GLFWwindow *window, int width, int height);
  void initWindow();

  int width = 0;
  int height = 0;
  bool framebufferResized = false;

  std::string windowName;
  GLFWwindow *window;
};
} // namespace bb
