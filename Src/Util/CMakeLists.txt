# CMakeList.txt : CMake project for VulkanTest, include source and define
# project specific logic here.
cmake_minimum_required (VERSION 3.8)
project ("Util" VERSION 0.1 LANGUAGES CXX DESCRIPTION "Utils")

add_library(${PROJECT_NAME}
"DXC_Compiler.cpp"
"GLSL_Compiler.cpp"
"ShaderCompiler.cpp"
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
		# $<$<CONFIG:RELEASE>:-O2>
		# $<$<CONFIG:DEBUG>:-O2>
        )
    if (${CMAKE_CXX_COMPILER_ID} STREQUAL "AppleClang" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
	   target_compile_options(${PROJECT_NAME}
			PRIVATE 
            # dxc compiler
			-fms-extensions
		)
	endif()
endif()

# libs
if(APPLE)
    find_package(Vulkan REQUIRED MoltenVK)
    target_link_libraries(${PROJECT_NAME}
        PRIVATE
        {Vulkan_MoltenVK_LIBRARY}
    )
else()
    find_package(Vulkan REQUIRED dxc shaderc_combined)
endif()

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
target_link_libraries(${PROJECT_NAME}
PRIVATE
${Vulkan_LIBRARIES}
${Vulkan_shaderc_combined_LIBRARY}
${Vulkan_dxc_LIBRARY}
${GLFW_LIBRARY}
)
