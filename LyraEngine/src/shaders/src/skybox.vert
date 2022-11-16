#version 450

layout(location = 0) in vec3 inPosition;

layout(binding = 0) uniform CameraData {
    mat4 proj;
    mat4 model;
} ubo;

layout(location = 0) out vec3 outUVW;

void main() {
    outUVW = inPosition;
    outUVW.xy *= -1;
    gl_Position = ubo.proj * vec4(inPosition, 1.0);
}
