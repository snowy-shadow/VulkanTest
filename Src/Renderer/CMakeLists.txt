# CMakeList.txt : CMake project for VulkanTest, include source and define
# project specific logic here.

# Add source to this project's executable.

cmake_minimum_required (VERSION 3.8)
project ("Renderer" VERSION 0.1 LANGUAGES CXX DESCRIPTION "Vulkan frontend")

add_library(${PROJECT_NAME}
"GraphicPipelineConfig.cpp"
"Image.cpp"
"Renderer.cpp"
"SwapChain.cpp"
"Window.cpp"
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${PROJECT_NAME} 
  PROPERTY CXX_STANDARD 20)
endif()

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
		-Werror
		-Wpedantic
		-O2
        # DXC 
        -fms-extensions
        -Wno-language-extension-token
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
    "Include"
)

target_include_directories(${PROJECT_NAME} 
    SYSTEM PRIVATE
    ${Vulkan_INCLUDE_DIR}
    ${GLFW_INCLUDE_DIRS} 
)

# Requires Util lib
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Util
    Device
    ${Vulkan_LIBRARIES}
    ${GLFW_LIBRARY}
)
