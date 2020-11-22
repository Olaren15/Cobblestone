#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "Graphics/GPU/GPU.hpp"
#include "Graphics/Memory/Image/Image.hpp"
#include "Graphics/Render/Window/RenderWindow.hpp"

namespace cbl::gfx {

struct SwapchainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities{};
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;

  SwapchainSupportDetails() = default;
  SwapchainSupportDetails(SwapchainSupportDetails const &swapChainSupportDetails);
  explicit SwapchainSupportDetails(VkPhysicalDevice const &physicalDevice,
                                   VkSurfaceKHR const &surface);
  ~SwapchainSupportDetails() = default;

  [[nodiscard]] bool isUsable() const;
};

struct Swapchain {
private:
  mem::MemoryManager &mMemoryManager;
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
  VkRenderPass renderPass{};
  SwapchainSupportDetails swapchainSupportDetails{};
  VkSwapchainKHR swapchain{};
  std::vector<mem::Image> frameBufferImages{};
  mem::Image depthBufferImage{};
  std::vector<VkFramebuffer> framebuffers{};

  Swapchain() = delete;
  Swapchain(Swapchain const &swapchain) = delete;
  Swapchain(GPU const &gpu, RenderWindow const &window, mem::MemoryManager &memoryManager);
  ~Swapchain();

  void handleFrameBufferResize(RenderWindow const &window);

  [[nodiscard]] float getAspectRatio() const;
  [[nodiscard]] bool isNotZeroPixels(RenderWindow const &window) const;
};
} // namespace cbl::gfx
