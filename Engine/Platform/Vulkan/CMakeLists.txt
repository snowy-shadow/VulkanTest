target_sources(${PROJECT_NAME}
	PUBLIC
	FILE_SET CXX_MODULES
	FILES
		Attachment.cppm
		Swapchain.cppm
		Context.cppm
		Device.cppm
		"Synchronization.cppm"
	PRIVATE
		Context.cpp
		Swapchain.cpp

		"Vulkan.h"
)

add_subdirectory(Native)
