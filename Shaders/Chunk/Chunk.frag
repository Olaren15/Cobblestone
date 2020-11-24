#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inUVW;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2DArray texSampler;

void main() {
    outColor = texture(texSampler, inUVW);
}