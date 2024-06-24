target_sources(${PROJECT_NAME}
	PUBLIC
	FILE_SET CXX_MODULES
	FILES
		DebugMessenger.cppm
		# Device.cppm
		PhysicalDevice.cppm
	PRIVATE
	# Device.cpp
	PhysicalDevice.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
