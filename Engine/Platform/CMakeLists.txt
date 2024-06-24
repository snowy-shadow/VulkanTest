target_sources(${PROJECT_NAME}
	PUBLIC
	FILE_SET CXX_MODULES
	FILES
	    Platform.cppm
	PRIVATE
)
add_subdirectory(Windows)
add_subdirectory(GLFW)
add_subdirectory(Vulkan)

