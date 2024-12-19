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

			# should be private
		Shader.cppm
		Pipeline.cppm
		Device.cppm
		RenderPass.cppm
		RendererStructure.cppm
)

target_sources(${PROJECT_NAME}
	PRIVATE
		RendererContext.cpp
		Renderer.cpp
		ProjectionCamera.cpp
		CameraController.cpp
)
# add_subdirectory(Resource)
