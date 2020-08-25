#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "graphics/RenderWindow.hpp"
#include "graphics/vulkan/QueueFamilyIndices.hpp"
#include "graphics/vulkan/SwapchainSupportDetails.hpp"

namespace flex {
struct Swapchain {
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
  SwapchainSupportDetails swapchainSupportDetails{};
  VkSwapchainKHR vulkanSwapchain{};
  std::vector<VkImage> images;
  VkFormat format{};
  VkExtent2D extent{};
  std::vector<VkImageView> imageViews;
  std::vector<VkFramebuffer> framebuffers;

  Swapchain() = default;
  Swapchain(Swapchain const &swapchain) = delete;

  void createSwapchain(VkPhysicalDevice const &physicalDevice, VkDevice const &device,
                       RenderWindow const &window, VkSurfaceKHR const &surface,
                       QueueFamilyIndices const &queueFamilyIndices);

  void createFrameBuffers(VkDevice const &device, VkRenderPass renderPass);

  void destroy(VkDevice const &device) const;
};
} // namespace flex
