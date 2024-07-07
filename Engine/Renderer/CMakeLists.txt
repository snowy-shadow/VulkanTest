target_sources(${PROJECT_NAME}
	PUBLIC
	FILE_SET CXX_MODULES
	FILES
		RendererContext.cppm

		Buffer.cppm

		RendererOption.cppm
		Renderer.cppm

		Camera.cppm
		OrthographicCamera.cppm

	PRIVATE
		RendererContext.cpp
		Renderer.cpp
		OrthographicCamera.cpp
)
