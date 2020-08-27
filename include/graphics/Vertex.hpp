#pragma once

#include <array>

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

namespace flex {
struct Vertex {
  glm::vec2 position;
  glm::vec3 color;

  static VkVertexInputBindingDescription getVulkanBindingDescription();
  static std::array<VkVertexInputAttributeDescription, 2> getVulkanAttributeDescriptions();
};
} // namespace flex