#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "GPU.hpp"
#include "Image.hpp"
#include "QueueFamiliIndices.hpp"
#include "SwapchainSupportDetails.hpp"
#include "graphics/RenderWindow.hpp"

namespace flex {
struct Swapchain {
private:
  MemoryManager *mMemoryManager{};

  [[nodiscard]] static VkPresentModeKHR
  getSupportedSwapchainPresentMode(std::vector<VkPresentModeKHR> const &availablePresentModes);
  [[nodiscard]] static VkExtent2D getSwapchainExtent(VkSurfaceCapabilitiesKHR const &capabilities,
                                                     RenderWindow const &window);

public:
  SwapchainSupportDetails swapchainSupportDetails{};
  VkSwapchainKHR swapchain{};
  std::vector<Image> frameBufferImages{};
  Image depthBufferImage{};
  std::vector<VkFramebuffer> framebuffers{};

  Swapchain() = default;
  Swapchain(Swapchain const &swapchain) = delete;

  [[nodiscard]] static VkSurfaceFormatKHR
  getSupportedSwapchainSurfaceFormat(SwapchainSupportDetails const &swapchainSupportDetails);
  [[nodiscard]] static VkFormat getSupportedDepthBufferFormat(GPU const &gpu);

  void initialise(GPU const &gpu, RenderWindow const &window, VkRenderPass const &renderPass,
                  MemoryManager &memoryManager);

  void handleFrameBufferResize(GPU const &gpu, RenderWindow const &window,
                               VkRenderPass const &renderPass);

  [[nodiscard]] float getAspectRatio() const;
  [[nodiscard]] bool canBeResized(GPU const &gpu, RenderWindow const &window) const;

  void destroy(GPU const &gpu);
};
} // namespace flex
