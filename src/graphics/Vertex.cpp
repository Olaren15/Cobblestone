#include "graphics/Vertex.hpp"

namespace flex {
VkVertexInputBindingDescription Vertex::getVulkanBindingDescription() {
  VkVertexInputBindingDescription bindingDescription;

  bindingDescription.binding = 0;
  bindingDescription.stride = sizeof(Vertex);
  bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 2> Vertex::getVulkanAttributeDescriptions() {
  std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

  // position
  attributeDescriptions[0].binding = 0;
  attributeDescriptions[0].location = 0;
  attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
  attributeDescriptions[0].offset = offsetof(Vertex, position);

  // color
  attributeDescriptions[1].binding = 0;
  attributeDescriptions[1].location = 1;
  attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
  attributeDescriptions[1].offset = offsetof(Vertex, color);

  return attributeDescriptions;
}
} // namespace flex