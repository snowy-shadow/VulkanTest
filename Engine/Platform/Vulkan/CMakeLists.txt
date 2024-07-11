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
		Buffer.cppm
		Util.cppm
		Shader.cppm

		RendererContext.cppm
	PRIVATE
		Swapchain.cpp
		"Vulkan.h"
		"Pipeline.cpp"
		Buffer.cpp
		Shader.cpp
		RendererContext.cpp
)

add_subdirectory(Native)
