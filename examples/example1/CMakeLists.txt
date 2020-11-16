ADD_EXECUTABLE(
		example1
		example1.cpp
		shaders/BlockShaderInfo.cpp
)

TARGET_LINK_LIBRARIES(example1 PRIVATE flex-engine)