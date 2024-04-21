# CMakeList.txt : CMake project for VulkanTest, include source and define
# project specific logic here.

add_executable (
	${PROJECT_NAME}
		"VulkanTest.cpp"
		"App.cpp"
)

set_target_properties(${PROJECT_NAME} 
	PROPERTIES 
	CXX_STANDARD 20
	RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}
)
  

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
	PRIVATE
		"Include"
)
target_include_directories(${PROJECT_NAME} 
    SYSTEM PRIVATE
		${Vulkan_INCLUDE_DIR}
		${GLFW_INCLUDE_DIRS} 
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE
		VulkanEngine
		${GLFW_LIBRARY}
)
