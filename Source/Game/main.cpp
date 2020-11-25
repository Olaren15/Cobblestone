#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define SDL_MAIN_HANDLED

#include "Graphics/Engine/Engine.hpp"

#include "Game/Chunks/Generator/ChunkGenerator.hpp"

void setupScene(cbl::gfx::Engine &rendererEngine, cbl::World &scene) {}

int main() {
  cbl::gfx::Engine renderEngine{};

  cbl::Chunk chunk = cbl::ChunkGenerator::generate();

  cbl::World world;
  world.meshes.push_back(chunk.mesh);

  renderEngine.loadWorld(world);

  renderEngine.run();

  renderEngine.unloadWorld();

  return EXIT_SUCCESS;
}
