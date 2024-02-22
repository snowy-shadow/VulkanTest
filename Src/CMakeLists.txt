# CMakeList.txt : CMake project for VulkanTest, include source and define
# project specific logic here.

# Add source to this project's executable.
add_executable (${PROJECT_NAME}
"VulkanTest.cpp"
"App.cpp"
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_target_properties(${PROJECT_NAME} 
	  PROPERTIES 
	  CXX_STANDARD 20
	  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
	  SUFFIX ".exe"
  )
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

# custom libs
add_subdirectory(Util)
add_subdirectory(Device)
add_subdirectory(Renderer)

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


target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Util
    Renderer
    Device
    ${Vulkan_LIBRARIES}
    ${GLFW_LIBRARY}
)

