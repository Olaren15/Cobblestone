add_executable(example1 example1.cpp)

if(MSVC)
  target_compile_options(example1 PRIVATE /W4 /WX)
else()
  target_compile_options(example1 PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

target_link_libraries(example1 PRIVATE flex-engine)
