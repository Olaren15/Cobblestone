#pragma once

#include <array>

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

namespace cbl::gfx {
struct Vertex {
  glm::vec3 position;
  glm::vec3 color;
  glm::vec2 uv;

  static VkVertexInputBindingDescription getVulkanBindingDescription();
  static std::array<VkVertexInputAttributeDescription, 3> getVulkanAttributeDescriptions();
};
} // namespace cbl::gfx