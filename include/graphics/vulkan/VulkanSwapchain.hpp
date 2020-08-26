#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "graphics/RenderWindow.hpp"
#include "graphics/vulkan/VulkanQueueFamilyIndices.hpp"
#include "graphics/vulkan/VulkanSwapchainSupportDetails.hpp"

namespace flex {
struct VulkanSwapchain {
private:
  [[nodiscard]] VkSurfaceFormatKHR
  chooseSwapchainSurfaceFormat(std::vector<VkSurfaceFormatKHR> const &availableFormats) const;
  [[nodiscard]] VkPresentModeKHR
  chooseSwapchainPresentMode(std::vector<VkPresentModeKHR> const &availablePresentModes) const;
  [[nodiscard]] VkExtent2D chooseSwapchainExtent(VkSurfaceCapabilitiesKHR const &capabilities,
                                                 RenderWindow const &window) const;

  void retrieveSwapchainImages(VkDevice const &device);
  void createImageViews(VkDevice const &device);

public:
  VulkanSwapchainSupportDetails swapchainSupportDetails{};
  VkSwapchainKHR swapchain = nullptr;
  std::vector<VkImage> images;
  VkFormat format{};
  VkExtent2D extent{};
  std::vector<VkImageView> imageViews;
  std::vector<VkFramebuffer> framebuffers;

  VulkanSwapchain() = default;
  VulkanSwapchain(VulkanSwapchain const &swapchain) = delete;

  void createSwapchain(VkPhysicalDevice const &physicalDevice, VkDevice const &device,
                       RenderWindow const &window, VkSurfaceKHR const &surface,
                       VulkanQueueFamilyIndices const &queueFamilyIndices);

  void createFrameBuffers(VkDevice const &device, VkRenderPass renderPass);

  void handleFrameBufferResize(VkPhysicalDevice const &physicalDevice, VkDevice const &device,
                               RenderWindow const &window, VkSurfaceKHR const &surface,
                               VulkanQueueFamilyIndices const &queueFamilyIndices,
                               VkRenderPass const &renderPass);
  void destroy(VkDevice const &device) const;
};
} // namespace flex
