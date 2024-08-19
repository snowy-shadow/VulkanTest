target_sources(${PROJECT_NAME}
	PUBLIC
	FILE_SET CXX_MODULES
	FILES
		RendererType.cppm
		Renderer.cppm
		RendererContext.cppm
		"Buffer.cppm"
		Camera.cppm
		"ProjectionCamera.cppm"
		CameraController.cppm
		 "Texture.cppm"

	PRIVATE
		RendererContext.cpp
		Renderer.cpp
		"ProjectionCamera.cpp"
				CameraController.cpp
)

# add_subdirectory(Resource)
