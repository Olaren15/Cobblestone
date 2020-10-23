#include "graphics/openGL/OpenGLRenderer.hpp"

namespace flex {
OpenGLRenderer::OpenGLRenderer(RenderWindow const &window) : mRenderWindow(window) {

  glewExperimental = GL_TRUE;
  glewInit();

  float vertices[]{
      0.0f,  0.5f,  1.0f, 0.0f, 0.0f, //
      0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, //
      -0.5f, -0.5f, 0.0f, 0.0f, 1.0f  //
  };

  GLuint elements[] = {0, 1, 2};

  glGenVertexArrays(1, &mVAO);
  glBindVertexArray(mVAO);

  glGenBuffers(1, &mVBO);
  glBindBuffer(GL_ARRAY_BUFFER, mVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glGenBuffers(1, &mEBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

  GLint compileStatus{};
  mVertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(mVertexShader, 1, &mVertexSource, nullptr);
  glCompileShader(mVertexShader);
  glGetShaderiv(mVertexShader, GL_COMPILE_STATUS, &compileStatus);

  mFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(mFragmentShader, 1, &mFragmentSource, nullptr);
  glCompileShader(mFragmentShader);
  glGetShaderiv(mFragmentShader, GL_COMPILE_STATUS, &compileStatus);

  mShaderProgram = glCreateProgram();
  glAttachShader(mShaderProgram, mVertexShader);
  glAttachShader(mShaderProgram, mFragmentShader);
  glBindFragDataLocation(mShaderProgram, 0, "outColor");
  glLinkProgram(mShaderProgram);
  glUseProgram(mShaderProgram);

  GLint posAttrib = glGetAttribLocation(mShaderProgram, "position");
  glEnableVertexAttribArray(posAttrib);
  glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);

  GLint colorAttribute = glGetAttribLocation(mShaderProgram, "color");
  glEnableVertexAttribArray(colorAttribute);
  glVertexAttribPointer(colorAttribute, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(2 * sizeof(float)));
}

OpenGLRenderer::~OpenGLRenderer() {
  glDeleteProgram(mShaderProgram);
  glDeleteShader(mFragmentShader);
  glDeleteShader(mVertexShader);

  glDeleteBuffers(1, &mEBO);
  glDeleteBuffers(1, &mVBO);
  glDeleteVertexArrays(1, &mVAO);
}

void OpenGLRenderer::draw() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);
}

} // namespace flex