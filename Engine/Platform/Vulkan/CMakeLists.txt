target_sources(${PROJECT_NAME}
	PUBLIC
	FILE_SET CXX_MODULES
	FILES
		Attachment.cppm
		Swapchain.cppm
		Context.cppm
		Device.cppm
		"Synchronization.cppm"
		Pipeline.cppm
		Buffer.cppm
		Util.cppm
	PRIVATE
		Context.cpp
		Swapchain.cpp
		"Vulkan.h"
		"Pipeline.cpp"
		Buffer.cpp
)

add_subdirectory(Native)
