#include "graphics/Mesh.hpp"

#include <graphics/vulkan/VulkanMemoryManager.hpp>

namespace flex {
Mesh::Mesh(std::vector<uint32_t> const &indices, std::vector<Vertex> const &vertices) {
  mIndices = indices;
  mVertices = vertices;
}

void Mesh::updateBufferData() {
  if (mVulkanBuffer.has_value()) {
    mVulkanBuffer->memoryManager.updateMeshBuffer(mVulkanBuffer.value(), *this);
  }
}

void Mesh::setIndices(std::vector<uint32_t> const &indices) {
  mIndices = indices;
  updateBufferData();
}

void Mesh::setVertices(std::vector<Vertex> const &vertices) {
  mVertices = vertices;
  updateBufferData();
}

void Mesh::setVulkanBuffer(VulkanBuffer buffer) { mVulkanBuffer = buffer; }

std::vector<uint32_t> Mesh::getIndices() const { return mIndices; }
std::vector<Vertex> Mesh::getVertices() const { return mVertices; }
std::optional<VulkanBuffer> Mesh::getVulkanBuffer() const { return mVulkanBuffer; }
size_t Mesh::getIndicesSize() const { return sizeof(mIndices[0]) * mIndices.size(); }
size_t Mesh::getVerticesSize() const { return sizeof(mVertices[0]) * mVertices.size(); }

} // namespace flex
