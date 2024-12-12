#include "bb_renderer.hpp"
#include "vulkan/vulkan_core.h"
#include <stdexcept>

namespace bb {

BbRenderer::BbRenderer(BbWindow &window, BbDevice &device) : bbWindow{window}, bbDevice{device} {
  recreateSwapChain();
  createCommandBuffers();
}

BbRenderer::~BbRenderer() { freeCommandBuffers(); }

void BbRenderer::recreateSwapChain() {
  auto extent = bbWindow.getExtent();

  while (extent.width == 0 || extent.height == 0) {
    extent = bbWindow.getExtent();
    glfwWaitEvents();
  }

  vkDeviceWaitIdle(bbDevice.device());

  if (bbSwapChain == nullptr) {
    bbSwapChain = std::make_unique<BbSwapChain>(bbDevice, extent);
  } else {
    std::shared_ptr<BbSwapChain> oldSwapChain = std::move(bbSwapChain);
    bbSwapChain = std::make_unique<BbSwapChain>(bbDevice, extent, oldSwapChain);

    if (!bbSwapChain->compareSwapFormats(*bbSwapChain.get())) {
      throw std::runtime_error("Swap chain image (or depth) format has changes!");
    }
  }

  // we'll come back
}

void BbRenderer::createCommandBuffers() {
  commandBuffers.resize(BbSwapChain::MAX_FRAMES_IN_FLIGHT);

  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = bbDevice.getCommandPool();
  allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

  if (vkAllocateCommandBuffers(bbDevice.device(), &allocInfo, commandBuffers.data()) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to allocate command buffers!");
  }
}

void BbRenderer::freeCommandBuffers() {
  vkFreeCommandBuffers(bbDevice.device(), bbDevice.getCommandPool(),
                       static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());

  commandBuffers.clear();
}

VkCommandBuffer BbRenderer::beginFrame() {
  assert(!isFrameStarted && "Can't call beginFrame while already in progress");
  auto result = bbSwapChain->acquireNextImage(&currentImageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    recreateSwapChain();
    return nullptr;
  }

  if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("failed to acquire swap chain image!");
  }

  isFrameStarted = true;

  auto commandBuffer = getCurrentCommandBuffer();

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
    std::runtime_error("failed to begin recording command buffer!");
  }

  return commandBuffer;
}

void BbRenderer::endFrame() {
  assert(isFrameStarted && "Can't call endFrame while frame is not in progress!");
  auto commandBuffer = getCurrentCommandBuffer();
  if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to record command buffer!");
  }

  auto result = bbSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
      bbWindow.wasWindowResized()) {
    bbWindow.resetWindowResizedFlag();
    recreateSwapChain();
  } else if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to present swap chain image!");
  }

  isFrameStarted = false;
  currentFrameIndex = (currentFrameIndex + 1) % BbSwapChain::MAX_FRAMES_IN_FLIGHT;
}

void BbRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
  assert(isFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
  assert(commandBuffer == getCurrentCommandBuffer() &&
         "Can't begin render pass on command buffer on a different frame");

  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = bbSwapChain->getRenderPass();
  renderPassInfo.framebuffer = bbSwapChain->getFrameBuffer(currentImageIndex);

  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = bbSwapChain->getSwapChainExtent();

  std::array<VkClearValue, 2> clearValues{};
  clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
  clearValues[1].depthStencil = {1.0f, 0};
  renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
  renderPassInfo.pClearValues = clearValues.data();

  vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(bbSwapChain->getSwapChainExtent().width);
  viewport.height = static_cast<float>(bbSwapChain->getSwapChainExtent().height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  VkRect2D scissor{{0, 0}, bbSwapChain->getSwapChainExtent()};
  vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
  vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void BbRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
  assert(isFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress");
  assert(commandBuffer == getCurrentCommandBuffer() &&
         "Can't end render pass on command buffer on a different frame");

  vkCmdEndRenderPass(commandBuffer);
}

} // namespace bb
