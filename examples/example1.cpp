#include <iostream>

#include <flexEngine.hpp>

int main() {

  flex::Configuration config;

  int width = config.getInt("display.window.width");
  int height = config.getInt("display.window.height");
  bool fullscreen = config.getBool("display.window.fullscreen");

  flex::Window window("window", width, height, fullscreen);

  while (!window.shouldExit()) {
    window.update();

    if (flex::InputHandler::keyPressed("Escape")) {
      std::cout << "ree" << std::endl;
    }
  }

  return 0;
}