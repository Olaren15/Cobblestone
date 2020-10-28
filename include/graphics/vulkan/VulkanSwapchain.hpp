#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "graphics/RenderWindow.hpp"
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
  [[nodiscard]] static VkFormat
  getSupportedDepthBufferFormat(VkPhysicalDevice const &physicalDevice);

  void createSwapchain(VkPhysicalDevice const &physicalDevice, VkDevice const &device,
                       RenderWindow const &window, VkSurfaceKHR const &surface,
                       VkRenderPass const &renderPass,
                       VulkanQueueFamilyIndices const &queueFamilyIndices,
                       VulkanMemoryManager &memoryManager);

  void handleFrameBufferResize(VkPhysicalDevice const &physicalDevice, VkDevice const &device,
                               RenderWindow const &window, VkSurfaceKHR const &surface,
                               VulkanQueueFamilyIndices const &queueFamilyIndices,
                               VkRenderPass const &renderPass);
  void destroy(VkDevice const &device);
};
} // namespace flex
