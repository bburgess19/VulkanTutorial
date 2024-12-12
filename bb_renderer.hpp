#pragma once

#include "bb_swap_chain.hpp"
#include "bb_window.hpp"

#include <cassert>
#include <memory>
#include <vector>

namespace bb {
class BbRenderer {
public:
  BbRenderer(BbWindow &window, BbDevice &device);
  ~BbRenderer();

  BbRenderer(const BbRenderer &) = delete;
  BbRenderer &operator=(const BbRenderer &) = delete;

  VkRenderPass getSwapChainRenderPass() const { return bbSwapChain->getRenderPass(); }
  float getAspectRatio() const { return bbSwapChain->extentAspectRatio(); }
  bool isFrameInProgress() const { return isFrameStarted; }

  VkCommandBuffer getCurrentCommandBuffer() const {
    assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
    return commandBuffers[currentFrameIndex];
  }

  int getFrameIndex() const {
    assert(isFrameStarted && "Cannot get frae index when frame not in progress");
    return currentFrameIndex;
  }

  VkCommandBuffer beginFrame();
  void endFrame();
  void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
  void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

private:
  void createCommandBuffers();
  void freeCommandBuffers();
  void recreateSwapChain();

  BbWindow &bbWindow;
  BbDevice &bbDevice;
  std::unique_ptr<BbSwapChain> bbSwapChain;
  std::vector<VkCommandBuffer> commandBuffers;

  uint32_t currentImageIndex;
  int currentFrameIndex{0};
  bool isFrameStarted{false};
};
} // namespace bb
