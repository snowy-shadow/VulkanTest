target_sources(${PROJECT_NAME}
	PUBLIC
	FILE_SET CXX_MODULES
	FILES
	    Buffer.cppm
	    GraphicsContext.cppm

	    RenderCmdAPI.cppm
	    Renderer.cppm

	    Camera.cppm

	PRIVATE
	    Renderer.cpp
)
