find_package(libconfig++ REQUIRED)

add_executable(${PROJECT_NAME}
  main.cpp
  utils/configuration.cpp
)

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)
endif()


target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBCONFIG++_LIBRARIES})