cmake_minimum_required (VERSION 3.8)
project ("Renderer" VERSION 0.1 LANGUAGES CXX DESCRIPTION "Vulkan front end")

add_library(${PROJECT_NAME}
"ImageGroup.cpp"
"Renderer.cpp"
"Swapchain.cpp"
"Window.cpp"
  )

set_property(TARGET ${PROJECT_NAME} 
PROPERTY CXX_STANDARD 20)

if (MSVC)
    # msvc is special like that
   target_compile_options(${PROJECT_NAME} 
		PRIVATE
		/W4
		/WX
		/permissive
	)
else()
    # additional warnings
   target_compile_options(${PROJECT_NAME} 
		PRIVATE
		-Wall 
		-Wextra
		-Werror
		-Wpedantic
		-O2
		# $<$<CONFIG:RELEASE>:-O2>
		# $<$<CONFIG:DEBUG>:-O2>
	)
endif()

# libs
find_package(Vulkan REQUIRED)

# GLFW
find_package(glfw3 3.3 REQUIRED)

target_include_directories(${PROJECT_NAME} 
	PUBLIC 
	${PROJECT_SOURCE_DIR}/include
)

target_include_directories(${PROJECT_NAME} 
	SYSTEM PUBLIC
    ${Vulkan_INCLUDE_DIR}
    ${GLFW_INCLUDE_DIRS} 
)

# Requires Util lib
target_link_libraries(${PROJECT_NAME}
	PUBLIC
		Util
		Device
		${GLFW_LIBRARY}
		${Vulkan_LIBRARIES}
)
