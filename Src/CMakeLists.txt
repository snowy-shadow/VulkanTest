# CMakeList.txt : CMake project for VulkanTest, include source and define
# project specific logic here.

# Add source to this project's executable.
add_executable (${PROJECT_NAME}
"VulkanTest.cpp"
"DXC_Compiler.h" "DXC_Compiler.cpp"
"App.h" "App.cpp"
"Window.h" "Window.cpp"
"Renderer.h" "Renderer.cpp" 
"Pipeline.h" "Pipeline.cpp"
"Instance.h" "Instance.cpp"
"PhysicalDevice.h" "PhysicalDevice.cpp" 
"SwapChain.h" "SwapChain.cpp"
"Image.h" "Image.cpp"
"File.h" 
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${PROJECT_NAME} 
  PROPERTY CXX_STANDARD 20)
endif()

if (MSVC)
    # msvc is special like that
    add_compile_options(
		/Wall
		/WX
		/permissive
	)
else()
    # additional warnings
    add_compile_options(
		-Wall 
		-Werror 
		-Wpedantic
		-O2
		# $<$<CONFIG:RELEASE>:-O2>
		# $<$<CONFIG:DEBUG>:-O2>
	)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	add_compile_options(
		-fms-extensions # dxc compiler 
	)
endif()

# libs
find_package(Vulkan REQUIRED OPTIONAL_COMPONENTS dxc)
find_package(glfw3 REQUIRED)

target_include_directories(${PROJECT_NAME} 
PUBLIC 
${Vulkan_INCLUDE_DIR} 
${GLFW_INCLUDE_DIRS} 
)
target_link_libraries(${PROJECT_NAME}
${Vulkan_LIBRARIES}
${Vulkan_dxc_LIBRARY}
${GLFW_LIBRARIES}
)

