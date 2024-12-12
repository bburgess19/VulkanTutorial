
#pragma once

#include "bb_camera.hpp"
#include "bb_game_object.hpp"
#include "bb_pipeline.hpp"

#include <memory>
#include <vector>

namespace bb {
class SimpleRenderSystem {
public:
  SimpleRenderSystem(BbDevice &device, VkRenderPass renderPass);
  ~SimpleRenderSystem();

  SimpleRenderSystem(const SimpleRenderSystem &) = delete;
  SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;

  void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<BbGameObject> &gameObjects,
                         const BbCamera &camera);

private:
  void createPipeline(VkRenderPass renderPass);
  void createPipelineLayout();
  void sierpinski(std::vector<BbModel::Vertex> &vertices, int depth, glm::vec2 left,
                  glm::vec2 right, glm::vec2 top);
  BbDevice &bbDevice;

  std::unique_ptr<BbPipeline> bbPipeline;
  VkPipelineLayout pipelineLayout;
};
} // namespace bb
