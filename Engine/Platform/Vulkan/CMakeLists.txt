target_sources(${PROJECT_NAME}
	PUBLIC
	FILE_SET CXX_MODULES
	FILES
		Attachment.cppm
		Swapchain.cppm
		"RendererContext.cppm"
		Device.cppm
		"Synchronization.cppm"
		"VulkanBuffer.cppm"
		DescriptorSet.cppm
		Util.cppm
		Shader.cppm
		VulkanImage.cppm
		VulkanTexture.cppm

# make these private
		RendererContext.cppm
		"PipelineManager.cppm"
		"TypeConversion.cppm"
)

target_sources(${PROJECT_NAME}
	PRIVATE
		Swapchain.cpp
		"Vulkan.h"
		"PipelineManager.cpp"
		VulkanBuffer.cpp
		DescriptorSet.cpp
		Shader.cpp
		VulkanImage.cpp
		VulkanTexture.cpp

		RendererContext.cpp
)

add_subdirectory(Native)
