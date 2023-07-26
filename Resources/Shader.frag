#version 450

layout(binding = 1) uniform sampler2D TextureSampler;

layout(location = 0) in vec3 FragColor;
layout(location = 1) in vec2 FragTextureCoord;

layout(location = 0) out vec4 OutColor;

void main() 
{
    OutColor = texture(TextureSampler, FragTextureCoord);
}