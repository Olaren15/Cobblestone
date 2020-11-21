#include "graphics/Vertex.hpp"

namespace flex {
VkVertexInputBindingDescription Vertex::getVulkanBindingDescription() {
  VkVertexInputBindingDescription bindingDescription;

  bindingDescription.binding = 0;
  bindingDescription.stride = static_cast<uint32_t>(sizeof(Vertex));
  bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 3> Vertex::getVulkanAttributeDescriptions() {
  std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

  // position
  attributeDescriptions[0].binding = 0;
  attributeDescriptions[0].location = 0;
  attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
  attributeDescriptions[0].offset = static_cast<uint32_t>(offsetof(Vertex, position));

  // color
  attributeDescriptions[1].binding = 0;
  attributeDescriptions[1].location = 1;
  attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
  attributeDescriptions[1].offset = static_cast<uint32_t>(offsetof(Vertex, color));

  // uv
  attributeDescriptions[2].binding = 0;
  attributeDescriptions[2].location = 2;
  attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
  attributeDescriptions[2].offset = static_cast<uint32_t>(offsetof(Vertex, uv));

  return attributeDescriptions;
}
} // namespace flex