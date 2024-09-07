target_sources(${PROJECT_NAME}
	PUBLIC
	FILE_SET CXX_MODULES
	FILES
		"RendererEnum.cppm"
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
 "Shader.cppm" "Pipeline.cppm" "RendererStructure.cppm")

# add_subdirectory(Resource)
