target_sources(${PROJECT_NAME}
	PUBLIC
	FILE_SET CXX_MODULES
	FILES
		Attachment.cppm
		Swapchain.cppm
		"RendererContext.cppm"
		Device.cppm
		"Synchronization.cppm"
		Pipeline.cppm
		"VulkanBuffer.cppm"
		DescriptorSet.cppm
		Util.cppm
		Shader.cppm
		VulkanImage.cppm
		VulkanTexture.cppm

		RendererContext.cppm

	PRIVATE
		Swapchain.cpp
		"Vulkan.h"
		"Pipeline.cpp"
		VulkanBuffer.cpp
				DescriptorSet.cpp
		Shader.cpp
		VulkanImage.cpp
		RendererContext.cpp
		VulkanTexture.cpp
)

add_subdirectory(Native)
