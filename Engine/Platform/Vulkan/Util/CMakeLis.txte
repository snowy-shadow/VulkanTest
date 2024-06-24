# CMakeList.txt : CMake project for VulkanTest, include source and define
# project specific logic here.
cmake_minimum_required (VERSION 3.8)
project ("Util" VERSION 0.1 LANGUAGES CXX DESCRIPTION "Utils")

add_library(${PROJECT_NAME}
    "DXC_Compiler.cpp"
    "GLSL_Compiler.cpp"
    "ShaderCompiler.cpp"
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
		PUBLIC
			# DXC 
			-fms-extensions
			-Wno-language-extension-token
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

# ========================
#           Find Libs
# ========================

# libs
find_package(Vulkan REQUIRED dxc shaderc_combined)
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

target_link_libraries(${PROJECT_NAME}
	PUBLIC
		${Vulkan_shaderc_combined_LIBRARY}
		${Vulkan_dxc_LIBRARY}
		${GLFW_LIBRARY}
)
