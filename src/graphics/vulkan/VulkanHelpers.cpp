#include "graphics/vulkan/VulkanHelpers.hpp"

#include <stdexcept>

void flex::validateVkResult(VkResult const &result) {
#ifndef NDEBUG
  switch (result) {
  case VK_SUCCESS:
  case VK_NOT_READY:
  case VK_TIMEOUT:
  case VK_EVENT_SET:
  case VK_EVENT_RESET:
  case VK_INCOMPLETE:
  case VK_SUBOPTIMAL_KHR:
  case VK_ERROR_OUT_OF_DATE_KHR: // handled
  case VK_THREAD_IDLE_KHR:
  case VK_THREAD_DONE_KHR:
  case VK_OPERATION_DEFERRED_KHR:
  case VK_OPERATION_NOT_DEFERRED_KHR:
  case VK_PIPELINE_COMPILE_REQUIRED_EXT:
    // Ok
    break;
  case VK_ERROR_OUT_OF_HOST_MEMORY:
    throw std::runtime_error("Failed to allocate host memory");
  case VK_ERROR_OUT_OF_DEVICE_MEMORY:
    throw std::runtime_error("Failed to allocate device memory");
  case VK_ERROR_INITIALIZATION_FAILED:
    throw std::runtime_error("Failed to initialize Vulkan object");
  case VK_ERROR_DEVICE_LOST:
    throw std::runtime_error("Vulkan device was lost");
  case VK_ERROR_MEMORY_MAP_FAILED:
    throw std::runtime_error("Failed to map memory");
  case VK_ERROR_LAYER_NOT_PRESENT:
    throw std::runtime_error("One or more requested Vulkan layer(s) is not available");
  case VK_ERROR_EXTENSION_NOT_PRESENT:
    throw std::runtime_error("One or more requested Vulkan extension(s) is not available");
  case VK_ERROR_FEATURE_NOT_PRESENT:
    throw std::runtime_error("One or more requested Vulkan feature(s) is not available");
  case VK_ERROR_INCOMPATIBLE_DRIVER:
    throw std::runtime_error("Driver is not compatible with requested version of vulkan");
  case VK_ERROR_TOO_MANY_OBJECTS:
    throw std::runtime_error("Too many objects of the same type have been created");
  case VK_ERROR_FORMAT_NOT_SUPPORTED:
    throw std::runtime_error("Requested format is not supported");
  case VK_ERROR_FRAGMENTED_POOL:
    throw std::runtime_error("Failed to allocate pool memory due to fragmentation");
  case VK_ERROR_OUT_OF_POOL_MEMORY:
    throw std::runtime_error("Failed to allocate pool memory");
  case VK_ERROR_INVALID_EXTERNAL_HANDLE:
    throw std::runtime_error("External handle is invalid");
  case VK_ERROR_FRAGMENTATION:
    throw std::runtime_error("Failed to allocate descriptor pool due to fragmentation");
  case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
    throw std::runtime_error(
        "Failed to allocate memory because it is either unavailable or no longer valid");
  case VK_ERROR_SURFACE_LOST_KHR:
    throw std::runtime_error("Surface has been lost");
  case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
    throw std::runtime_error("The requested window is already used by Vulkan or another API");
  case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
    throw std::runtime_error("Display used by swapchain is not compatible");
  case VK_ERROR_VALIDATION_FAILED_EXT:
    throw std::runtime_error("Validation failed");
  case VK_ERROR_INVALID_SHADER_NV:
    throw std::runtime_error("The shader is invalid");
  case VK_ERROR_INCOMPATIBLE_VERSION_KHR:
    throw std::runtime_error("Incompatible version");
  case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
    throw std::runtime_error("Invalid DRM format modifier plane layout");
  case VK_ERROR_NOT_PERMITTED_EXT:
    throw std::runtime_error("Not permitted");
  case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
    throw std::runtime_error("Fullscreen exclusive mode lost");
  case VK_ERROR_UNKNOWN:
    throw std::runtime_error("Unknown error");
  case VK_RESULT_MAX_ENUM:
    throw std::runtime_error("VkResult max enum");
  }
#endif
}
