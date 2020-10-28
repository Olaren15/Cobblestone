#pragma once

#include <vector>

#include <GL/glew.h>

#include "graphics/RenderWindow.hpp"

namespace flex {
struct OpenGLRenderer {
private:
  const char *mVertexSource = R"glsl(
#version 150 core

in vec2 position;
in vec3 color;

out vec3 Color;

void main()
{
    Color = color;
    gl_Position = vec4(position, 0.0, 1.0);
}
)glsl";

  const char *mFragmentSource = R"glsl(
#version 150 core

in vec3 Color;

out vec4 outColor;

void main()
{
    outColor = vec4(Color, 1.0);
}
)glsl";
  GLuint mVertexShader{};
  GLuint mFragmentShader{};
  GLuint mShaderProgram{};
  GLuint mVAO{};
  GLuint mVBO{};
  GLuint mEBO{};

public:
  OpenGLRenderer();
  ~OpenGLRenderer();

  void draw();
};
} // namespace flex