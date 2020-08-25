#include "graphics/vulkan/SwapChain.hpp"

#include <set>

namespace flex {
VkSurfaceFormatKHR SwapChain::chooseSwapChainSurfaceFormat(
    std::vector<VkSurfaceFormatKHR> const &availableFormats) const {
  if (availableFormats.empty()) {
    throw std::runtime_error("Cannot choose a format from an empty array");
  }

  for (VkSurfaceFormatKHR const &availableFormat : availableFormats) {
    if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
        availableFormat.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR) {
      return availableFormat;
    }
  }

  return availableFormats.front();
}

VkPresentModeKHR SwapChain::chooseSwapChainPresentMode(
    std::vector<VkPresentModeKHR> const &availablePresentModes) const {
  for (VkPresentModeKHR const &availablePresentMode : availablePresentModes) {
    if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
      return availablePresentMode;
    }
  }

  return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D SwapChain::chooseSwapChainExtent(VkSurfaceCapabilitiesKHR const &capabilities,
                                            RenderWindow const &window) const {
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

void SwapChain::createSwapChain(VkDevice const &device, VkSurfaceKHR const &surface,
                                VkSurfaceFormatKHR const &surfaceFormat,
                                VkPresentModeKHR const &presentMode,
                                QueueFamilyIndices const &queueFamilyIndices,
                                VkExtent2D const &extent,
                                VkSurfaceCapabilitiesKHR const &surfaceCapabilities,
                                VkSwapchainKHR const &oldSwapChain) {

  uint32_t const minimumImageCount =
      surfaceCapabilities.maxImageCount > 0
          ? std::min(surfaceCapabilities.minImageCount + 1, surfaceCapabilities.maxImageCount)
          : surfaceCapabilities.minImageCount + 1;

  std::vector<uint32_t> uniqueQueueFamilies{queueFamilyIndices.graphics.value(),
                                            queueFamilyIndices.transfer.value(),
                                            queueFamilyIndices.present.value()};
  // remove duplicate queues
  uniqueQueueFamilies.erase(std::unique(uniqueQueueFamilies.begin(), uniqueQueueFamilies.end()),
                            uniqueQueueFamilies.end());

  VkSwapchainCreateInfoKHR swapchainCreateInfo{};
  swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapchainCreateInfo.surface = surface;
  swapchainCreateInfo.minImageCount = minimumImageCount;
  swapchainCreateInfo.imageFormat = surfaceFormat.format;
  swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
  swapchainCreateInfo.imageExtent = extent;
  swapchainCreateInfo.imageArrayLayers = 1;
  swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  swapchainCreateInfo.imageSharingMode =
      uniqueQueueFamilies.size() > 1 ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE;
  swapchainCreateInfo.queueFamilyIndexCount = static_cast<uint32_t>(uniqueQueueFamilies.size());
  swapchainCreateInfo.pQueueFamilyIndices = uniqueQueueFamilies.data();
  swapchainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
  swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapchainCreateInfo.presentMode = presentMode;
  swapchainCreateInfo.clipped = VK_TRUE;
  swapchainCreateInfo.oldSwapchain = oldSwapChain;

  vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapChain);
}

void SwapChain::retrieveSwapChainImages(VkDevice const &device) {
  uint32_t swapChainImageCount;
  vkGetSwapchainImagesKHR(device, swapChain, &swapChainImageCount, nullptr);
  images.resize(swapChainImageCount);
  vkGetSwapchainImagesKHR(device, swapChain, &swapChainImageCount, images.data());
}

void SwapChain::createImageViews(VkDevice const &device) {
  imageViews.reserve(images.size());

  VkImageSubresourceRange subresourceRange;
  subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  subresourceRange.baseMipLevel = 0;
  subresourceRange.levelCount = 1;
  subresourceRange.baseArrayLayer = 0;
  subresourceRange.layerCount = 1;

  VkComponentMapping componentMapping;
  componentMapping.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  componentMapping.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  componentMapping.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  componentMapping.a = VK_COMPONENT_SWIZZLE_IDENTITY;

  for (VkImage const &image : images) {
    VkImageViewCreateInfo imageViewCreateInfo{};
    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.image = image;
    imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewCreateInfo.format = format;
    imageViewCreateInfo.components = componentMapping;
    imageViewCreateInfo.subresourceRange = subresourceRange;

    VkImageView imageView;
    vkCreateImageView(device, &imageViewCreateInfo, nullptr, &imageView);

    imageViews.push_back(imageView);
  }
}

SwapChain::SwapChain(VkDevice const &device, RenderWindow const &window,
                     VkSurfaceKHR const &surface,
                     SwapChainSupportDetails const &swapChainSupportDetails,
                     QueueFamilyIndices const &queueFamilyIndices) {

  VkSurfaceFormatKHR const surfaceFormat =
      chooseSwapChainSurfaceFormat(swapChainSupportDetails.formats);

  format = surfaceFormat.format;

  VkPresentModeKHR const presentMode =
      chooseSwapChainPresentMode(swapChainSupportDetails.presentModes);

  extent = chooseSwapChainExtent(swapChainSupportDetails.capabilities, window);

  createSwapChain(device, surface, surfaceFormat, presentMode, queueFamilyIndices, extent,
                  swapChainSupportDetails.capabilities, {});

  retrieveSwapChainImages(device);
  createImageViews(device);
}

void SwapChain::createFrameBuffers(VkDevice const &device, VkRenderPass renderPass) {
  framebuffers.reserve(imageViews.size());

  for (VkImageView const &imageView : imageViews) {

    VkFramebufferCreateInfo framebufferCreateInfo{};
    framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferCreateInfo.renderPass = renderPass;
    framebufferCreateInfo.attachmentCount = 1;
    framebufferCreateInfo.pAttachments = &imageView;
    framebufferCreateInfo.width = extent.width;
    framebufferCreateInfo.height = extent.height;
    framebufferCreateInfo.layers = 1;

    VkFramebuffer framebuffer;
    vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &framebuffer);
    framebuffers.push_back(framebuffer);
  }
}

void SwapChain::destroy(VkDevice const &device) const {
  for (VkFramebuffer const &framebuffer : framebuffers) {
    vkDestroyFramebuffer(device, framebuffer, nullptr);
  }

  for (VkImageView const &imageView : imageViews) {
    vkDestroyImageView(device, imageView, nullptr);
  }

  vkDestroySwapchainKHR(device, swapChain, nullptr);
};

} // namespace flex
