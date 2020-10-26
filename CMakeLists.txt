CMAKE_MINIMUM_REQUIRED(VERSION 3.2)
SET(CMAKE_CXX_STANDARD 17)
SET(OpenGL_GL_PREFERENCE GLVND)

PROJECT(flex-engine VERSION 1 LANGUAGES CXX)

FIND_PACKAGE(libconfig++ REQUIRED)
FIND_PACKAGE(SDL2 REQUIRED)
FIND_PACKAGE(Vulkan REQUIRED)
FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(GLEW REQUIRED)
FIND_PACKAGE(glm REQUIRED)

ADD_LIBRARY(
		flex-engine STATIC
		src/core/InputHandler.cpp
		src/core/Time.cpp
		src/core/Vector2.cpp
		src/graphics/openGL/OpenGLRenderer.cpp
		src/graphics/vulkan/vk_mem_alloc.cpp
		src/graphics/vulkan/VulkanBuffer.cpp
		src/graphics/vulkan/VulkanHelpers.cpp
		src/graphics/vulkan/VulkanMemoryManager.cpp
		src/graphics/vulkan/VulkanPipeline.cpp
		src/graphics/vulkan/VulkanQueueFamilyIndices.cpp
		src/graphics/vulkan/VulkanQueues.cpp
		src/graphics/vulkan/VulkanRenderer.cpp
		src/graphics/vulkan/VulkanSwapchain.cpp
		src/graphics/vulkan/VulkanSwapchainSupportDetails.cpp
		src/graphics/Camera.cpp
		src/graphics/Mesh.cpp
		src/graphics/RenderWindow.cpp
		src/graphics/Vertex.cpp
		src/utils/Configuration.cpp
)

TARGET_LINK_LIBRARIES(
		flex-engine PRIVATE
		config++
		SDL2::SDL2
		Vulkan::Vulkan
		OpenGL::GL
		GLEW::GLEW
		glm
)

TARGET_INCLUDE_DIRECTORIES(flex-engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

SET_TARGET_PROPERTIES(flex-engine PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION 1)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)

ADD_SUBDIRECTORY(examples)