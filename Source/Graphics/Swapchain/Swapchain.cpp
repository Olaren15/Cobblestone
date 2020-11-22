#include "Swapchain.hpp"

#include <algorithm>
#include <stdexcept>

#include "Graphics/Memory/MemoryManager/MemoryManager.hpp"
#include "Graphics/Utils/VulkanHelpers.hpp"

namespace cbl::gfx {

SwapchainSupportDetails::SwapchainSupportDetails(
    SwapchainSupportDetails const &swapchainSupportDetails) {
  capabilities = swapchainSupportDetails.capabilities;
  formats = swapchainSupportDetails.formats;
  presentModes = swapchainSupportDetails.presentModes;
}

SwapchainSupportDetails::SwapchainSupportDetails(VkPhysicalDevice const &physicalDevice,
                                                 VkSurfaceKHR const &surface) {
  validateVkResult(
      vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities));

  uint32_t vectorLength;
  validateVkResult(
      vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &vectorLength, nullptr));
  formats.resize(vectorLength);
  validateVkResult(
      vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &vectorLength, formats.data()));

  validateVkResult(
      vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &vectorLength, nullptr));
  presentModes.resize(vectorLength);
  validateVkResult(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &vectorLength,
                                                             presentModes.data()));
}

bool SwapchainSupportDetails::isUsable() const {
  return !(formats.empty() || presentModes.empty());
}

Swapchain::Swapchain(const GPU &gpu, const RenderWindow &window, mem::MemoryManager &memoryManager)
    : mMemoryManager{memoryManager}, mGPU{gpu} {
  swapchainSupportDetails = SwapchainSupportDetails{mGPU.physicalDevice, mGPU.renderSurface};
  createRenderPass();
  createSwapchain(window);
}

Swapchain::~Swapchain() {
  cleanSwapchain();
  vkDestroySwapchainKHR(mGPU.device, swapchain, nullptr);
  vkDestroyRenderPass(mGPU.device, renderPass, nullptr);
}

VkPresentModeKHR
Swapchain::chooseSwapchainPresentMode(std::vector<VkPresentModeKHR> const &availablePresentModes,
                                      bool const &vsync) {
  if (vsync) {
    return VK_PRESENT_MODE_FIFO_KHR;
  }

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

  return mem::Image::findSupportedFormat(gpu, preferredFormats, VK_IMAGE_TILING_OPTIMAL,
                                         VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

void Swapchain::createRenderPass() {
  VkAttachmentDescription colorAttachment{};
  colorAttachment.format = getSupportedSwapchainSurfaceFormat(swapchainSupportDetails).format;
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference colorAttachmentReference;
  colorAttachmentReference.attachment = 0;
  colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription depthAttachment{};
  depthAttachment.format = getSupportedDepthBufferFormat(mGPU);
  depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depthAttachmentReference{};
  depthAttachmentReference.attachment = 1;
  depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpassDescription{};
  subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpassDescription.colorAttachmentCount = 1;
  subpassDescription.pColorAttachments = &colorAttachmentReference;
  subpassDescription.pDepthStencilAttachment = &depthAttachmentReference;

  VkSubpassDependency subpassDependency{};
  subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  subpassDependency.dstSubpass = 0;
  subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  subpassDependency.srcAccessMask = 0;
  subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
  VkRenderPassCreateInfo renderPassCreateInfo{};
  renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
  renderPassCreateInfo.pAttachments = attachments.data();
  renderPassCreateInfo.subpassCount = 1;
  renderPassCreateInfo.pSubpasses = &subpassDescription;
  renderPassCreateInfo.dependencyCount = 1;
  renderPassCreateInfo.pDependencies = &subpassDependency;

  validateVkResult(vkCreateRenderPass(mGPU.device, &renderPassCreateInfo, nullptr, &renderPass));
}

void Swapchain::createSwapchain(RenderWindow const &window) {

  VkSurfaceFormatKHR const surfaceFormat =
      getSupportedSwapchainSurfaceFormat(swapchainSupportDetails);

  VkFormat format = surfaceFormat.format;

  VkPresentModeKHR const presentMode =
      chooseSwapchainPresentMode(swapchainSupportDetails.presentModes, !mGPU.isDedicated());

  VkExtent2D extent = getSwapchainExtent(swapchainSupportDetails.capabilities, window);

  uint32_t const minimumImageCount =
      swapchainSupportDetails.capabilities.maxImageCount > 0
          ? std::min(swapchainSupportDetails.capabilities.minImageCount + 1,
                     swapchainSupportDetails.capabilities.maxImageCount)
          : swapchainSupportDetails.capabilities.minImageCount + 1;

  std::set<uint32_t> uniqueQueueFamilyIndices{mGPU.queueFamilyIndices.graphics.value(),
                                              mGPU.queueFamilyIndices.present.value()};
  std::vector<uint32_t> queueFamiliesIndices{uniqueQueueFamilyIndices.begin(),
                                             uniqueQueueFamilyIndices.end()};

  VkSwapchainCreateInfoKHR swapchainCreateInfo{};
  swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapchainCreateInfo.surface = mGPU.renderSurface;
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

  validateVkResult(vkCreateSwapchainKHR(mGPU.device, &swapchainCreateInfo, nullptr, &swapchain));

  uint32_t swapchainImageCount;
  validateVkResult(vkGetSwapchainImagesKHR(mGPU.device, swapchain, &swapchainImageCount, nullptr));
  std::vector<VkImage> swapChainImages{swapchainImageCount};
  validateVkResult(vkGetSwapchainImagesKHR(mGPU.device, swapchain, &swapchainImageCount,
                                           swapChainImages.data()));

  frameBufferImages.resize(swapChainImages.size());
  for (size_t i = 0; i < swapChainImages.size(); i++) {
    frameBufferImages[i].image = swapChainImages[i];
    frameBufferImages[i].format = format;
    frameBufferImages[i].extent = extent;
    mMemoryManager.createImageView(frameBufferImages[i], VK_IMAGE_ASPECT_COLOR_BIT);
  }

  depthBufferImage = mMemoryManager.createImage(
      frameBufferImages[0].extent, getSupportedDepthBufferFormat(mGPU), VK_IMAGE_TILING_OPTIMAL,
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
        vkCreateFramebuffer(mGPU.device, &framebufferCreateInfo, nullptr, &framebuffers[i]));
  }
}

void Swapchain::cleanSwapchain() {

  mGPU.waitIdle();

  for (VkFramebuffer const &framebuffer : framebuffers) {
    vkDestroyFramebuffer(mGPU.device, framebuffer, nullptr);
  }

  for (mem::Image image : frameBufferImages) {
    vkDestroyImageView(mGPU.device, image.imageView, nullptr);
  }
  mMemoryManager.destroyImage(depthBufferImage);
}

void Swapchain::handleFrameBufferResize(RenderWindow const &window) {
  cleanSwapchain();

  swapchainSupportDetails = SwapchainSupportDetails{mGPU.physicalDevice, mGPU.renderSurface};

  VkSwapchainKHR oldSwapchain = swapchain;
  createSwapchain(window);

  vkDestroySwapchainKHR(mGPU.device, oldSwapchain, nullptr);
}

float Swapchain::getAspectRatio() const {
  return static_cast<float>(frameBufferImages[0].extent.width) /
         static_cast<float>(frameBufferImages[0].extent.height);
}

bool Swapchain::isNotZeroPixels(RenderWindow const &window) const {

  VkExtent2D extent = getSwapchainExtent(swapchainSupportDetails.capabilities, window);

  return extent.height != 0 && extent.width != 0;
}

} // namespace cbl::gfx
