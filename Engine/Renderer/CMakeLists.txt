target_sources(${PROJECT_NAME}
	PUBLIC
	FILE_SET CXX_MODULES
	FILES
		RendererContext.cppm

		"Buffer.cppm"

		"RendererType.cppm"
		Renderer.cppm

		Camera.cppm
		"ProjectionCamera.cppm"

	PRIVATE
		RendererContext.cpp
		Renderer.cpp
		"ProjectionCamera.cpp"
)

add_subdirectory(Resource)
