target_sources(${PROJECT_NAME}
	PUBLIC
	FILE_SET CXX_MODULES
	FILES
	    Platform.cppm
)
add_subdirectory(Windows)
