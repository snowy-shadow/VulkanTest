target_sources(${PROJECT_NAME}
	PUBLIC
	FILE_SET CXX_MODULES
	FILES
		Context.cppm
	PRIVATE
		Context.cpp
)

add_subdirectory(Device)
