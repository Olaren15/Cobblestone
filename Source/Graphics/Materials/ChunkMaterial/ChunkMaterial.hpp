#pragma once

#include "Graphics/Materials/BaseMaterial.hpp"

namespace cbl::gfx {
struct ChunkMaterial : public BaseMaterial {
private:
  mem::Texture texture{};

public:
  ChunkMaterial() = delete;
  ChunkMaterial(GPU const &gpu, mem::MemoryManager &memoryManager, BaseShader const *shader);
  ~ChunkMaterial() override;
};
} // namespace cbl::gfx
