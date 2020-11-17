CMAKE_MINIMUM_REQUIRED(VERSION 3.2)
SET(CMAKE_CXX_STANDARD 17)

PROJECT(flex-engine VERSION 1 LANGUAGES CXX)

FIND_PACKAGE(SDL2 REQUIRED)
FIND_PACKAGE(Vulkan REQUIRED)
FIND_PACKAGE(glm REQUIRED)

ADD_LIBRARY(
		flex-engine STATIC
		src/core/InputHandler.cpp
		src/core/Scene.cpp
		src/core/Time.cpp
		src/core/Vector2.cpp
		src/graphics/vk_mem_alloc.cpp
		src/graphics/Buffer.cpp
		src/graphics/CommandBufferRecorder.cpp
		src/graphics/Frame.cpp
		src/graphics/GPU.cpp
		src/graphics/VulkanHelpers.cpp
		src/graphics/Image.cpp
		src/graphics/MemoryManager.cpp
		src/graphics/QueueFamiliIndices.cpp
		src/graphics/RendererEngine.cpp
		src/graphics/Shader.cpp
		src/graphics/ShaderInformation.cpp
		src/graphics/Swapchain.cpp
		src/graphics/SwapchainSupportDetails.cpp
		src/graphics/Camera.cpp
		src/graphics/Mesh.cpp
		src/graphics/RenderWindow.cpp
		src/graphics/Vertex.cpp
)

TARGET_LINK_LIBRARIES(
		flex-engine PRIVATE
		SDL2::SDL2
		Vulkan::Vulkan
		glm
		pthread
)

TARGET_INCLUDE_DIRECTORIES(flex-engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

SET_TARGET_PROPERTIES(flex-engine PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION 1)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)

ADD_SUBDIRECTORY(examples)