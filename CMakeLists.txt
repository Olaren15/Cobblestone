cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_STANDARD 17)

project(flex-engine VERSION 0.0.1 LANGUAGES CXX)

find_package(libconfig++ REQUIRED)
find_package(SDL2 REQUIRED)

add_library(flex-engine SHARED
  src/core/eventHandler.cpp
  src/graphics/window.cpp
  src/utils/configuration.cpp
)

if(MSVC)
  target_compile_options(flex-engine PRIVATE /W4 /WX)
else()
  target_compile_options(flex-engine PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

target_link_libraries(flex-engine PRIVATE config++ SDL2::SDL2)

target_include_directories(flex-engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include>)

set_target_properties(flex-engine PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION 1)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(examples)
