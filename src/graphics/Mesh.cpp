#include "graphics/Mesh.hpp"

namespace flex {
size_t Mesh::getIndicesSize() const { return sizeof(uint32_t) * indices.size(); }

size_t Mesh::getVerticesSize() const { return sizeof(Vertex) * vertices.size(); }
} // namespace flex
