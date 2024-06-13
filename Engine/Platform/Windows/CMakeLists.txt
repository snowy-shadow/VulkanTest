target_sources(${PROJECT_NAME}
	PUBLIC
	FILE_SET CXX_MODULES
	FILES
		PlatformWindows.cppm
		Window.cppm
	PRIVATE
		Window.cpp
)
