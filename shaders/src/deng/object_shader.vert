#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObj {
    mat4 view;
    mat4 projection;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexPosition;

layout(location = 1) out vec2 fragTexPos;

void main() {
    gl_Position = ubo.projection * ubo.view * vec4(inPosition[0], inPosition[1], inPosition[2], 1.0f);
    fragTexPos = inTexPosition;
}