#include <iostream>
#include <string>

#include <libconfig.h++>

#include "utils/configuration.hpp"

int main() {

  flex::Configuration config;

  int width = config.getInt("display.window.width");

  int height = config.getInt("display.window.height");

  bool fullscreen = config.getBool("display.window.fullscreen");

  std::cout << "Width : " << width << std::endl
            << "Height : " << height << std::endl
            << "Fullscreen : " << (fullscreen ? "true" : "false") << std::endl;

  return 0;
}