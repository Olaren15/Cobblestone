CMAKE_MINIMUM_REQUIRED(VERSION 3.2)
SET(CMAKE_CXX_STANDARD 17)

SET(PROJECT_NAME Cobblestone)

PROJECT(${PROJECT_NAME} VERSION 1 LANGUAGES CXX)

FIND_PACKAGE(SDL2 REQUIRED)
FIND_PACKAGE(Vulkan REQUIRED)
FIND_PACKAGE(glm REQUIRED)

ADD_EXECUTABLE(
		${PROJECT_NAME}

		Source/Core/Input/Input.cpp
		Source/Core/Time/Time.cpp
		Source/Core/World/World.cpp

		Source/External/stb_image/stb_image.cpp
		Source/External/vk_mem_alloc/vk_mem_alloc.cpp
		Source/External/imgui/imgui.cpp
		Source/External/imgui/imgui_demo.cpp
		Source/External/imgui/imgui_draw.cpp
		Source/External/imgui/backends/imgui_impl_sdl.cpp
		Source/External/imgui/backends/imgui_impl_vulkan.cpp
		Source/External/imgui/imgui_widgets.cpp
		Source/External/PerlinNoise/PerlinNoise.cpp

		Source/Game/Block/Block.cpp
		Source/Game/Chunks/Generator/ChunkGenerator.cpp
		Source/Game/Chunks/Chunk.cpp
		Source/Game/main.cpp

		Source/Graphics/Camera/Camera.cpp
		Source/Graphics/Vertex/Vertex.cpp
		Source/Graphics/CommandBufferRecorder/CommandBufferRecorder.cpp
		Source/Graphics/Engine/Engine.cpp
		Source/Graphics/Frame/Frame.cpp
		Source/Graphics/GPU/GPU.cpp
		Source/Graphics/Materials/ChunkMaterial/ChunkMaterial.cpp
		Source/Graphics/Materials/BaseMaterial.cpp
		Source/Graphics/Memory/Buffer/Buffer.cpp
		Source/Graphics/Memory/Image/Image.cpp
		Source/Graphics/Memory/MemoryManager/MemoryManager.cpp
		Source/Graphics/Memory/Texture/Texture.cpp
		Source/Graphics/Mesh/Mesh.cpp
		Source/Graphics/Shaders/ChunkShader/ChunkShader.cpp
		Source/Graphics/Shaders/BaseShader.cpp
		Source/Graphics/Swapchain/Swapchain.cpp
		Source/Graphics/Window/Window.cpp
		Source/Graphics/Utils/VulkanHelpers.cpp

		Source/Math/Vector/Vector2/Vector2.cpp
)

IF (APPLE)
	INCLUDE_DIRECTORIES(/usr/local/include)
	TARGET_LINK_LIBRARIES(${PROJECT_NAME} PRIVATE glm::glm)
ELSEIF (NOT WINDOWS)
	TARGET_LINK_LIBRARIES(${PROJECT_NAME} PRIVATE glm::glm)
ELSE()
	TARGET_LINK_LIBRARIES(${PROJECT_NAME} PRIVATE glm)
ENDIF ()

IF (NOT MSVC)
	TARGET_LINK_LIBRARIES(${PROJECT_NAME} PRIVATE pthread)
ENDIF ()

TARGET_LINK_LIBRARIES(
		${PROJECT_NAME} PRIVATE
		SDL2::SDL2
		Vulkan::Vulkan
)

TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Source ${CMAKE_CURRENT_SOURCE_DIR}/Source/External/imgui)