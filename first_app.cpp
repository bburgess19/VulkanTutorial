#include "first_app.hpp"
#include "bb_camera.hpp"
#include "bb_game_object.hpp"
#include "keyboard_movement_controller.hpp"
#include "simple_render_system.hpp"
#include "vulkan/vulkan_core.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <chrono>

namespace bb {

FirstApp::FirstApp() { loadGameObjects(); }

FirstApp::~FirstApp() {}

void FirstApp::run() {
  SimpleRenderSystem simpleRenderSystem{bbDevice, bbRenderer.getSwapChainRenderPass()};
  BbCamera camera{};

  auto viewerObject = BbGameObject::createGameObject();
  KeyboardMovementController cameraController{};

  auto currentTime = std::chrono::high_resolution_clock::now();

  while (!bbWindow.shouldClose()) {
    glfwPollEvents();

    auto newTime = std::chrono::high_resolution_clock::now();
    float frameTime =
        std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
    currentTime = newTime;

    cameraController.moveInPlaneXZ(bbWindow.getGLFWwindow(), frameTime, viewerObject);
    camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

    float aspect = bbRenderer.getAspectRatio();
    camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 10.0f);

    if (auto commandBuffer = bbRenderer.beginFrame()) {
      bbRenderer.beginSwapChainRenderPass(commandBuffer);
      simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
      bbRenderer.endSwapChainRenderPass(commandBuffer);
      bbRenderer.endFrame();
    }

    vkDeviceWaitIdle(bbDevice.device());
  }
}

void FirstApp::loadGameObjects() {
  std::shared_ptr<BbModel> bbModel = BbModel::createModelFromFile(bbDevice, "models/flat_vase.obj");

  auto gameObject = BbGameObject::createGameObject();
  gameObject.model = bbModel;
  gameObject.transform.translation = {0.0f, 0.5f, 2.5f};
  gameObject.transform.scale = glm::vec3(3.0f);
  gameObjects.push_back(std::move(gameObject));
}

} // namespace bb
