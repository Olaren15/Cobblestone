#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "graphics/RenderWindow.hpp"
#include "graphics/vulkan/VulkanGPU.hpp"
#include "graphics/vulkan/VulkanImage.hpp"
#include "graphics/vulkan/VulkanQueueFamilyIndices.hpp"
#include "graphics/vulkan/VulkanSwapchainSupportDetails.hpp"

namespace flex {
struct VulkanSwapchain {
private:
  VulkanMemoryManager &mMemoryManager;

  [[nodiscard]] static VkPresentModeKHR
  getSupportedSwapchainPresentMode(std::vector<VkPresentModeKHR> const &availablePresentModes);
  [[nodiscard]] static VkExtent2D getSwapchainExtent(VkSurfaceCapabilitiesKHR const &capabilities,
                                                     RenderWindow const &window);

public:
  VulkanSwapchainSupportDetails swapchainSupportDetails{};
  VkSwapchainKHR swapchain{};
  std::vector<VulkanImage> frameBufferImages;
  VulkanImage depthBufferImage;
  std::vector<VkFramebuffer> framebuffers;

  explicit VulkanSwapchain(VulkanMemoryManager &memoryManager);
  VulkanSwapchain(VulkanSwapchain const &swapchain) = delete;

  [[nodiscard]] static VkSurfaceFormatKHR
  getSupportedSwapchainSurfaceFormat(VulkanSwapchainSupportDetails const &swapchainSupportDetails);
  [[nodiscard]] static VkFormat getSupportedDepthBufferFormat(VulkanGPU const &gpu);

  void initialise(VulkanGPU const &gpu, RenderWindow const &window, VkRenderPass const &renderPass,
                  VulkanMemoryManager &memoryManager);

  void handleFrameBufferResize(VulkanGPU const &gpu, RenderWindow const &window,
                               VkRenderPass const &renderPass);

  [[nodiscard]] float getAspectRatio() const;
  [[nodiscard]] bool canBeResized(VulkanGPU const &gpu, RenderWindow const &window) const;

  void destroy(VulkanGPU const &gpu);
};
} // namespace flex
