#include "BaseMaterial.hpp"

namespace cbl::gfx {

BaseMaterial::BaseMaterial(GPU const &gpu, mem::MemoryManager &memoryManager,
                           BaseShader const *shader)
    : mMemoryManager{memoryManager} {}

} // namespace cbl::gfx
