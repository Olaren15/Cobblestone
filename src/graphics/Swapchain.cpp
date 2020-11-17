﻿#include "graphics/Swapchain.hpp"

#include <algorithm>
#include <stdexcept>

#include "graphics/MemoryManager.hpp"
#include "graphics/VulkanHelpers.hpp"

namespace flex {

VkPresentModeKHR Swapchain::getSupportedSwapchainPresentMode(
    std::vector<VkPresentModeKHR> const &availablePresentModes) {

  const std::set<VkPresentModeKHR> rankedModes{VK_PRESENT_MODE_MAILBOX_KHR,
                                               VK_PRESENT_MODE_IMMEDIATE_KHR};

  for (VkPresentModeKHR const &presentMode : rankedModes) {
    auto mode = std::find(availablePresentModes.begin(), availablePresentModes.end(), presentMode);
    if (mode != availablePresentModes.end()) {
      return *mode;
    }
  }

  // only guaranteed mode
  return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Swapchain::getSwapchainExtent(VkSurfaceCapabilitiesKHR const &capabilities,
                                               RenderWindow const &window) {
  if (capabilities.currentExtent.width != UINT32_MAX) {
    // Cannot decide on the extent size
    return capabilities.currentExtent;
  }

  VkExtent2D const windowDrawableSize = window.getDrawableVulkanSurfaceSize();

  uint32_t const actualWidth =
      std::clamp(windowDrawableSize.width, capabilities.minImageExtent.width,
                 capabilities.maxImageExtent.width);
  uint32_t const actualHeight =
      std::clamp(windowDrawableSize.height, capabilities.minImageExtent.height,
                 capabilities.maxImageExtent.height);

  return VkExtent2D{actualWidth, actualHeight};
}

VkSurfaceFormatKHR Swapchain::getSupportedSwapchainSurfaceFormat(
    SwapchainSupportDetails const &swapchainSupportDetails) {
  if (swapchainSupportDetails.formats.empty()) {
    throw std::runtime_error("Cannot choose a format from an empty array");
  }

  for (VkSurfaceFormatKHR const &availableFormat : swapchainSupportDetails.formats) {
    if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
        availableFormat.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR) {
      return availableFormat;
    }
  }

  return swapchainSupportDetails.formats.front();
}

VkFormat Swapchain::getSupportedDepthBufferFormat(GPU const &gpu) {
  std::vector<VkFormat> const preferredFormats{
      VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D16_UNORM_S8_UINT, VK_FORMAT_D32_SFLOAT_S8_UINT};

  return Image::findSupportedFormat(gpu, preferredFormats, VK_IMAGE_TILING_OPTIMAL,
                                          VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

void Swapchain::initialise(GPU const &gpu, RenderWindow const &window,
                                 VkRenderPass const &renderPass, MemoryManager &memoryManager) {
  mMemoryManager = &memoryManager;
  swapchainSupportDetails = SwapchainSupportDetails{gpu};

  VkSurfaceFormatKHR const surfaceFormat =
      getSupportedSwapchainSurfaceFormat(swapchainSupportDetails);

  VkFormat format = surfaceFormat.format;

  VkPresentModeKHR const presentMode =
      getSupportedSwapchainPresentMode(swapchainSupportDetails.presentModes);

  VkExtent2D extent = getSwapchainExtent(swapchainSupportDetails.capabilities, window);

  uint32_t const minimumImageCount =
      swapchainSupportDetails.capabilities.maxImageCount > 0
          ? std::min(swapchainSupportDetails.capabilities.minImageCount + 1,
                     swapchainSupportDetails.capabilities.maxImageCount)
          : swapchainSupportDetails.capabilities.minImageCount + 1;

  std::set<uint32_t> uniqueQueueFamilyIndices{gpu.queueFamilyIndices.graphics.value(),
                                              gpu.queueFamilyIndices.present.value()};
  std::vector<uint32_t> queueFamiliesIndices{uniqueQueueFamilyIndices.begin(),
                                             uniqueQueueFamilyIndices.end()};

  VkSwapchainCreateInfoKHR swapchainCreateInfo{};
  swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapchainCreateInfo.surface = gpu.renderSurface;
  swapchainCreateInfo.minImageCount = minimumImageCount;
  swapchainCreateInfo.imageFormat = surfaceFormat.format;
  swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
  swapchainCreateInfo.imageExtent = extent;
  swapchainCreateInfo.imageArrayLayers = 1;
  swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  if (queueFamiliesIndices.size() > 1)
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
  else
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  swapchainCreateInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueFamiliesIndices.size());
  swapchainCreateInfo.pQueueFamilyIndices = queueFamiliesIndices.data();
  swapchainCreateInfo.preTransform = swapchainSupportDetails.capabilities.currentTransform;
  swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapchainCreateInfo.presentMode = presentMode;
  swapchainCreateInfo.clipped = VK_TRUE;
  swapchainCreateInfo.oldSwapchain = swapchain;

  validateVkResult(vkCreateSwapchainKHR(gpu.device, &swapchainCreateInfo, nullptr, &swapchain));

  uint32_t swapchainImageCount;
  validateVkResult(vkGetSwapchainImagesKHR(gpu.device, swapchain, &swapchainImageCount, nullptr));
  std::vector<VkImage> swapChainImages{swapchainImageCount};
  validateVkResult(
      vkGetSwapchainImagesKHR(gpu.device, swapchain, &swapchainImageCount, swapChainImages.data()));

  frameBufferImages.resize(swapChainImages.size());
  for (size_t i = 0; i < swapChainImages.size(); i++) {
    frameBufferImages[i].image = swapChainImages[i];
    frameBufferImages[i].format = format;
    frameBufferImages[i].extent = extent;
    mMemoryManager->createImageView(frameBufferImages[i], VK_IMAGE_ASPECT_COLOR_BIT);
  }

  depthBufferImage = memoryManager.createImage(
      frameBufferImages[0].extent, getSupportedDepthBufferFormat(gpu), VK_IMAGE_TILING_OPTIMAL,
      VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_ASPECT_DEPTH_BIT);

  framebuffers.resize(frameBufferImages.size());

  for (size_t i = 0; i < framebuffers.size(); i++) {
    std::array<VkImageView, 2> attachments{frameBufferImages[i].imageView,
                                           depthBufferImage.imageView};

    VkFramebufferCreateInfo framebufferCreateInfo{};
    framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferCreateInfo.renderPass = renderPass;
    framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebufferCreateInfo.pAttachments = attachments.data();
    framebufferCreateInfo.width = frameBufferImages[i].extent.width;
    framebufferCreateInfo.height = frameBufferImages[i].extent.height;
    framebufferCreateInfo.layers = 1;

    validateVkResult(
        vkCreateFramebuffer(gpu.device, &framebufferCreateInfo, nullptr, &framebuffers[i]));
  }
}

void Swapchain::handleFrameBufferResize(GPU const &gpu, RenderWindow const &window,
                                              VkRenderPass const &renderPass) {
  if (mMemoryManager == nullptr) {
    throw std::runtime_error("swapchain needs to be initialised first!");
  }
  gpu.waitIdle();

  for (VkFramebuffer &framebuffer : framebuffers) {
    vkDestroyFramebuffer(gpu.device, framebuffer, nullptr);
  }

  for (Image image : frameBufferImages) {
    vkDestroyImageView(gpu.device, image.imageView, nullptr);
  }
  mMemoryManager->destroyImage(depthBufferImage);

  VkSwapchainKHR oldSwapchain = swapchain;
  initialise(gpu, window, renderPass, *mMemoryManager);

  vkDestroySwapchainKHR(gpu.device, oldSwapchain, nullptr);
}

float Swapchain::getAspectRatio() const {
  return static_cast<float>(frameBufferImages[0].extent.width) /
         static_cast<float>(frameBufferImages[0].extent.height);
}

bool Swapchain::canBeResized(GPU const &gpu, RenderWindow const &window) const {

  SwapchainSupportDetails supportDetails = SwapchainSupportDetails{gpu};
  VkExtent2D extent = getSwapchainExtent(supportDetails.capabilities, window);

  return extent.height != 0 && extent.width != 0;
}

void Swapchain::destroy(GPU const &gpu) {
  if (mMemoryManager == nullptr) {
    return;
  }

  for (VkFramebuffer const &framebuffer : framebuffers) {
    vkDestroyFramebuffer(gpu.device, framebuffer, nullptr);
  }

  for (Image image : frameBufferImages) {
    vkDestroyImageView(gpu.device, image.imageView, nullptr);
  }
  mMemoryManager->destroyImage(depthBufferImage);

  vkDestroySwapchainKHR(gpu.device, swapchain, nullptr);
}

} // namespace flex