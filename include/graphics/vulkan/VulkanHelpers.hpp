#pragma once

#include <vulkan/vulkan.h>

#include <vector>

#include "graphics/vulkan/VulkanGPU.hpp"

namespace flex {
void validateVkResult(VkResult const &result);

[[nodiscard]] unsigned int ratePhysicalDevice(VkPhysicalDevice const &physicalDevice,
                                              VkSurfaceKHR const &vulkanSurface,
                                              std::vector<char const *> const &requiredExtensions);
[[nodiscard]] bool physicalDeviceSupportsExtensions(VkPhysicalDevice const &physicalDevice,
                                                    std::vector<const char *> const &extensions);
} // namespace flex
