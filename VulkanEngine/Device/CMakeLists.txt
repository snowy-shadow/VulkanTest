# CMakeList.txt : CMake project for VulkanTest, include source and define
# project specific logic here.

# Add source to this project's executable.
cmake_minimum_required (VERSION 3.8)

project ("Device" VERSION 0.1 LANGUAGES CXX DESCRIPTION "Vulkan Device setup")

add_library(${PROJECT_NAME}
"Instance.cpp"
"Device.cpp"
"PhysicalDevice.cpp"
"DebugMessenger.cpp")

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
		${Vulkan_LIBRARIES}
		${GLFW_LIBRARY}
)
