#pragma once

#include <exception>
#include <string>
#include <utility>

namespace flex {
enum struct RenderAPI { OpenGL, Vulkan };

struct InvalidRenderAPIException final : virtual std::exception {
protected:
  const std::string mErrorMessage;

public:
  InvalidRenderAPIException()
      : mErrorMessage("The chosen render API does not support this feature") {}

  explicit InvalidRenderAPIException(std::string errorMessage)
      : mErrorMessage(std::move(errorMessage)) {}

  [[nodiscard]] const char *what() const noexcept override { return mErrorMessage.c_str(); }
};
} // namespace flex
