#include "graphics/vulkan/SwapChain.hpp"

#include <set>

namespace flex {
vk::SurfaceFormatKHR
SwapChain::chooseSwapChainSurfaceFormat(std::vector<vk::SurfaceFormatKHR> const &availableFormats) {
  if (availableFormats.empty()) {
    throw std::runtime_error("Cannot choose a format from an empty array");
  }

  for (vk::SurfaceFormatKHR const &availableFormat : availableFormats) {
    if (availableFormat.format == vk::Format::eB8G8R8A8Srgb &&
        availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
      return availableFormat;
    }
  }

  return availableFormats.front();
}

vk::PresentModeKHR SwapChain::chooseSwapChainPresentMode(
    std::vector<vk::PresentModeKHR> const &availablePresentModes) {
  for (vk::PresentModeKHR const &availablePresentMode : availablePresentModes) {
    if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
      return availablePresentMode;
    }
  }

  return vk::PresentModeKHR::eFifo;
}

vk::Extent2D SwapChain::chooseSwapChainExtent(vk::SurfaceCapabilitiesKHR const &capabilities,
                                              Window const &window) {
  if (capabilities.currentExtent.width != UINT32_MAX) {
    // Cannot decide on the extent size
    return capabilities.currentExtent;
  }

  vk::Extent2D const windowDrawableSize = window.getDrawableVulkanSurfaceSize();

  uint32_t const actualWidth =
      std::clamp(windowDrawableSize.width, capabilities.minImageExtent.width,
                 capabilities.maxImageExtent.width);
  uint32_t const actualHeight =
      std::clamp(windowDrawableSize.height, capabilities.minImageExtent.height,
                 capabilities.maxImageExtent.height);

  return vk::Extent2D{actualWidth, actualHeight};
}

vk::SwapchainKHR SwapChain::createSwapChain(vk::Device const &device, vk::SurfaceKHR const &surface,
                                            vk::SurfaceFormatKHR const &surfaceFormat,
                                            vk::PresentModeKHR const &presentMode,
                                            QueueFamilyIndices const &queueFamilyIndices,
                                            vk::Extent2D const &extent,
                                            vk::SurfaceCapabilitiesKHR const &surfaceCapabilities,
                                            vk::SwapchainKHR const &oldSwapChain) {

  uint32_t const minimumImageCount =
      surfaceCapabilities.maxImageCount > 0
          ? std::min(surfaceCapabilities.minImageCount + 1, surfaceCapabilities.maxImageCount)
          : surfaceCapabilities.minImageCount + 1;

  std::set<uint32_t> uniqueQueueFamilyIndices{queueFamilyIndices.graphics.value(),
                                              queueFamilyIndices.transfer.value(),
                                              queueFamilyIndices.present.value()};

  vk::SwapchainCreateInfoKHR const swapChainCreateInfo{
      {},
      surface,
      minimumImageCount,
      surfaceFormat.format,
      surfaceFormat.colorSpace,
      extent,
      1,
      vk::ImageUsageFlagBits::eColorAttachment,
      uniqueQueueFamilyIndices.size() > 1 ? vk::SharingMode::eConcurrent
                                          : vk::SharingMode::eExclusive,
      std::vector<uint32_t>{uniqueQueueFamilyIndices.begin(), uniqueQueueFamilyIndices.end()},
      surfaceCapabilities.currentTransform,
      vk::CompositeAlphaFlagBitsKHR::eOpaque,
      presentMode,
      true,
      oldSwapChain};

  return device.createSwapchainKHR(swapChainCreateInfo);
}

std::vector<vk::ImageView>
SwapChain::createImageViews(vk::Device const &device, std::vector<vk::Image> const &swapChainImages,
                            vk::Format const &swapChainImagesFormat) {
  std::vector<vk::ImageView> imageViews{};

  imageViews.reserve(swapChainImages.size());

  vk::ImageSubresourceRange const subResourceRange{vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1};

  for (vk::Image const &image : swapChainImages) {
    vk::ImageViewCreateInfo imageViewCreateInfo{{},
                                                image,
                                                vk::ImageViewType::e2D,
                                                swapChainImagesFormat,
                                                vk::ComponentMapping{},
                                                subResourceRange};

    imageViews.push_back(device.createImageView(imageViewCreateInfo));
  }
  return imageViews;
}

SwapChain::SwapChain(vk::Device const &device, Window const &window, vk::SurfaceKHR const &surface,
                     SwapChainSupportDetails const &swapChainSupportDetails,
                     QueueFamilyIndices const &queueFamilyIndices) {

  vk::SurfaceFormatKHR const surfaceFormat =
      chooseSwapChainSurfaceFormat(swapChainSupportDetails.formats);
  format = surfaceFormat.format;
  vk::PresentModeKHR const presentMode =
      chooseSwapChainPresentMode(swapChainSupportDetails.presentModes);
  extent = chooseSwapChainExtent(swapChainSupportDetails.capabilities, window);
  swapChain = createSwapChain(device, surface, surfaceFormat, presentMode, queueFamilyIndices,
                              extent, swapChainSupportDetails.capabilities, {});
  imageViews = createImageViews(device, images, format);
}

void SwapChain::createFrameBuffers(vk::Device const &device, vk::RenderPass renderPass) {
  framebuffers.reserve(imageViews.size());

  for (vk::ImageView const &imageView : imageViews) {
    std::array<vk::ImageView, 1> attachments{imageView};

    vk::FramebufferCreateInfo const framebufferCreateInfo{{},           renderPass,    attachments,
                                                          extent.width, extent.height, 1};

    framebuffers.push_back(device.createFramebuffer(framebufferCreateInfo));
  }
}

void SwapChain::destroy(vk::Device const &device) const {
  for (vk::Framebuffer const &framebuffer : framebuffers) {
    device.destroyFramebuffer(framebuffer);
  }

  for (vk::ImageView const &imageView : imageViews) {
    device.destroyImageView(imageView);
  }

  device.destroySwapchainKHR(swapChain);
}

} // namespace flex
