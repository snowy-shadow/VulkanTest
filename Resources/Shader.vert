#version 450

layout(binding = 0) uniform UnifromBufferObject
{
	mat4 Model;
	mat4 View;
	mat4 Projection;
} UBO;

// vertex attributes
layout(location = 0) in vec3 InPosition;
layout(location = 1) in vec3 InColor;
layout(location = 2) in vec2 InTextureCoord;

layout(location = 0) out vec3 FragColor;
layout(location = 1) out vec2 FragTextureCoord;

void main()
{
	gl_Position = UBO.Projection * UBO.View * UBO.Model * vec4(InPosition, 1.0);

	FragColor = InColor;
	FragTextureCoord = InTextureCoord;
}