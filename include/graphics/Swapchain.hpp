#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "GPU.hpp"
#include "Image.hpp"
#include "QueueFamilyIndices.hpp"
#include "SwapchainSupportDetails.hpp"
#include "graphics/RenderWindow.hpp"

namespace flex {
struct Swapchain {
private:
  MemoryManager &mMemoryManager;
  GPU const &mGPU;

  [[nodiscard]] static VkPresentModeKHR
  chooseSwapchainPresentMode(std::vector<VkPresentModeKHR> const &availablePresentModes,
                             bool const &vsync);
  [[nodiscard]] static VkExtent2D getSwapchainExtent(VkSurfaceCapabilitiesKHR const &capabilities,
                                                     RenderWindow const &window);

  [[nodiscard]] static VkSurfaceFormatKHR
  getSupportedSwapchainSurfaceFormat(SwapchainSupportDetails const &swapchainSupportDetails);
  [[nodiscard]] static VkFormat getSupportedDepthBufferFormat(GPU const &gpu);

  void createRenderPass();

  void createSwapchain(RenderWindow const &window);
  void cleanSwapchain();

public:
  VkRenderPass renderPass;
  SwapchainSupportDetails swapchainSupportDetails{};
  VkSwapchainKHR swapchain{};
  std::vector<Image> frameBufferImages{};
  Image depthBufferImage{};
  std::vector<VkFramebuffer> framebuffers{};

  Swapchain() = delete;
  Swapchain(Swapchain const &swapchain) = delete;
  Swapchain(GPU const &gpu, RenderWindow const &window, MemoryManager &memoryManager);
  ~Swapchain();

  void handleFrameBufferResize(RenderWindow const &window);

  [[nodiscard]] float getAspectRatio() const;
  [[nodiscard]] bool isNotZeroPixels(RenderWindow const &window) const;
};
} // namespace flex
