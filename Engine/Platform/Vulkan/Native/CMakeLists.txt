﻿target_sources(${PROJECT_NAME}
	PUBLIC
	FILE_SET CXX_MODULES
	FILES
		Instance.cppm
		PhysicalDevice.cppm
		Swapchain.cppm
		RenderPass.cppm
	PRIVATE
		Instance.cpp
		PhysicalDevice.cpp
		Swapchain.cpp
		RenderPass.cpp
)
