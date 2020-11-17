#include "graphics/Mesh.hpp"

#include <graphics/MemoryManager.hpp>

namespace flex {
Mesh::Mesh(std::vector<uint32_t> const &indices, std::vector<Vertex> const &vertices) {
  this->indices = indices;
  this->vertices = vertices;
}

size_t Mesh::getIndicesSize() const { return sizeof(indices[0]) * indices.size(); }
size_t Mesh::getVerticesSize() const { return sizeof(vertices[0]) * vertices.size(); }
size_t Mesh::getRequiredBufferSize() const { return getIndicesSize() + getVerticesSize(); }

} // namespace flex
