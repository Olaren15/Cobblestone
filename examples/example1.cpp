#include <iostream>

#include <flexEngine.hpp>

int flexMain() {
  const flex::Configuration config;

  const unsigned int width = config.getUnsignedInt("display.window.width");
  const unsigned int height = config.getUnsignedInt("display.window.height");
  const bool fullscreen = config.getBool("display.window.fullscreen");

  flex::Window window{"window", width, height, fullscreen};
  flex::VulkanRenderer vulkanRenderer{window};

  while (!window.shouldExit()) {
    window.update();

    if (flex::InputHandler::keyPressed("Space")) {
      std::cout << "pew pew !" << std::endl;
    }
  }

  return 0;
}
