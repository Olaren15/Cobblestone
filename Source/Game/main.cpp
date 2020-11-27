#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define SDL_MAIN_HANDLED

#include "Graphics/Engine/Engine.hpp"

#include "Game/Chunks/Generator/ChunkGenerator.hpp"

void setupScene(cbl::gfx::Engine &rendererEngine, cbl::World &scene) {}

int main() {
  cbl::gfx::Engine renderEngine{};

  auto chunks = cbl::ChunkGenerator::generateMany(5, 5);

  cbl::World world;
  for (auto const &chunk : chunks) {
    world.meshes.push_back(chunk.second.mesh);
  }

  renderEngine.loadWorld(world);

  renderEngine.run();

  renderEngine.unloadWorld();

  return EXIT_SUCCESS;
}
