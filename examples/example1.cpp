#include <iostream>

#include <flexEngine.hpp>

int flexMain() {
  const flex::Configuration config;

  const unsigned int width = config.getUnsignedInt("display.window.width");
  const unsigned int height = config.getUnsignedInt("display.window.height");
  const bool fullscreen = config.getBool("display.window.fullscreen");

  flex::Window window{"window", width, height, fullscreen};
  flex::VulkanRenderer vulkanRenderer{window};

  unsigned int i = 0;

  while (!window.shouldExit()) {
    window.update();

    if (flex::InputHandler::keyPressed("Space")) {
      std::cout << i++ << '\n';
    }
  }

#ifndef NDEBUG
  std::cout << "Press enter to continue.";
  std::cin.ignore();
#endif

  return 0;
}
