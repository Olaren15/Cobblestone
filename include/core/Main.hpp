#pragma once

#include <iostream>

int flexMain();

int main() {
  int returnValue;

  try {
    returnValue = flexMain();

  } catch (std::exception &ex) {
    std::cout << ex.what() << '\n';
    return EXIT_FAILURE;
  }

  return returnValue;
}
