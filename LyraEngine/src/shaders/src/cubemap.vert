#version 450

layout(location = 0) in vec3 inPosition;

layout(set = 0, binding = 0) uniform CameraData {
    mat4 proj;
    mat4 model;
} ubo;

// this is going to include the position of the cubemap, which the skybox won't have since it always follows the camera

layout(location = 0) out vec3 outUVW;

void main() {
    outUVW = inPosition;
    outUVW.xy *= -1;
    gl_Position = ubo.proj * ubo.model * vec4(inPosition, 1.0);
}
