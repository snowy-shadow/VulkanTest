# shared library
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

# spdlog
set(SPDLOG_BUILD_EXAMPLE OFF)
set(SPDLOG_BUILD_EXAMPLE_HO OFF)
set(SPDLOG_USE_STD_FORMAT ON)
add_subdirectory(spdlog)

# GLFW
option (GLFW_STANDALONE OFF)
option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
option(GLFW_INSTALL "Generate installation target" OFF)
option(GLFW_DOCUMENT_INTERNALS "Include internals in documentation" OFF)
add_subdirectory(GLFW)

# ImGui
target_sources(${PROJECT_NAME} PRIVATE
    ImGui/imgui.h
    ImGui/imgui_internal.h
    ImGui/imgui.cpp
    ImGui/imgui_demo.cpp
    ImGui/imgui_draw.cpp
    ImGui/imgui_tables.cpp
    ImGui/imgui_widgets.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE
    ImGui/
    ImGui/backends)

target_link_libraries(${PROJECT_NAME}
	PUBLIC
		spdlog::spdlog

	PRIVATE
		glfw
)
