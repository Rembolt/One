
#version 450

layout(location = 0) in vec3 fragColor;//doesn't need the same name

layout(location = 0) out vec4 outColor;//locatio->which framebuffer

void main() {//called for every fragment;
    outColor = vec4(fragColor, 1.0);
}