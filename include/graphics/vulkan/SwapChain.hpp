#pragma once

#include <vector>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan.hpp>

#include "graphics/RenderWindow.hpp"
#include "graphics/vulkan/QueueFamilyIndices.hpp"
#include "graphics/vulkan/SwapChainSupportDetails.hpp"

namespace flex {
struct SwapChain {
private:
  [[nodiscard]] VkSurfaceFormatKHR
  chooseSwapChainSurfaceFormat(std::vector<VkSurfaceFormatKHR> const &availableFormats) const;
  [[nodiscard]] VkPresentModeKHR
  chooseSwapChainPresentMode(std::vector<VkPresentModeKHR> const &availablePresentModes) const;
  [[nodiscard]] VkExtent2D chooseSwapChainExtent(VkSurfaceCapabilitiesKHR const &capabilities,
                                                 RenderWindow const &window) const;
  void createSwapChain(VkDevice const &device, VkSurfaceKHR const &surface,
                       VkSurfaceFormatKHR const &surfaceFormat, VkPresentModeKHR const &presentMode,
                       QueueFamilyIndices const &queueFamilyIndices, VkExtent2D const &extent,
                       VkSurfaceCapabilitiesKHR const &surfaceCapabilities,
                       VkSwapchainKHR const &oldSwapChain);

  void retrieveSwapChainImages(VkDevice const &device);
  void createImageViews(VkDevice const &device);

public:
  VkSwapchainKHR swapChain;
  std::vector<VkImage> images;
  VkFormat format;
  VkExtent2D extent;
  std::vector<VkImageView> imageViews;
  std::vector<VkFramebuffer> framebuffers;

  SwapChain() = default;
  SwapChain(VkDevice const &device, RenderWindow const &window, VkSurfaceKHR const &surface,
            SwapChainSupportDetails const &swapChainSupportDetails,
            QueueFamilyIndices const &queueFamilyIndices);

  void createFrameBuffers(VkDevice const &device, VkRenderPass renderPass);

  void destroy(VkDevice const &device) const;
};
} // namespace flex
