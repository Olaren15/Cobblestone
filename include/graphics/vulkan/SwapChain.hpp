#pragma once

#include <vector>

#include <vulkan/vulkan.hpp>

#include "graphics/vulkan/QueueFamilyIndices.hpp"
#include "graphics/vulkan/SwapChainSupportDetails.hpp"
#include "graphics/window.hpp"

namespace flex {
struct SwapChain {
private:
  [[nodiscard]] static vk::SurfaceFormatKHR
  chooseSwapChainSurfaceFormat(std::vector<vk::SurfaceFormatKHR> const &availableFormats);
  [[nodiscard]] static vk::PresentModeKHR
  chooseSwapChainPresentMode(std::vector<vk::PresentModeKHR> const &availablePresentModes);
  [[nodiscard]] static vk::Extent2D
  chooseSwapChainExtent(vk::SurfaceCapabilitiesKHR const &capabilities, Window const &window);
  [[nodiscard]] static vk::SwapchainKHR
  createSwapChain(vk::Device const &device, vk::SurfaceKHR const &surface,
                  vk::SurfaceFormatKHR const &surfaceFormat, vk::PresentModeKHR const &presentMode,
                  QueueFamilyIndices const &queueFamilyIndices, vk::Extent2D const &extent,
                  vk::SurfaceCapabilitiesKHR const &surfaceCapabilities,
                  vk::SwapchainKHR const &oldSwapChain);

  [[nodiscard]] static std::vector<vk::ImageView>
  createImageViews(vk::Device const &device, std::vector<vk::Image> const &swapChainImages,
                   vk::Format const &swapChainImagesFormat);

public:
  vk::SwapchainKHR swapChain;
  std::vector<vk::Image> images;
  vk::Format format;
  vk::Extent2D extent;
  std::vector<vk::ImageView> imageViews;

  SwapChain() = default;
  SwapChain(vk::Device const &device, Window const &window, vk::SurfaceKHR const &surface,
            SwapChainSupportDetails const &swapChainSupportDetails,
            QueueFamilyIndices const &queueFamilyIndices);

  void destroy(vk::Device const &device) const;
};
} // namespace flex
