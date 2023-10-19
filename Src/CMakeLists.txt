# CMakeList.txt : CMake project for VulkanTest, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (${PROJECT_NAME} 
"VulkanTest.cpp" 
"App.h" "App.cpp" 
"Window.h" "Window.cpp"
"Renderer.h" "Renderer.cpp" 
"Pipeline.h" "Pipeline.cpp"
"Instance.h" "Instance.cpp"
"PhysicalDevice.h" "PhysicalDevice.cpp" 
"SwapChain.h" "SwapChain.cpp"
)


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${PROJECT_NAME} 
  PROPERTY CXX_STANDARD 20)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

# Custom libraries
set(GLFW_PATH "S:/Dev/Dependencies/Libraries/glfw-3.3.7")
set(glm_DIR "S:/Dev/Dependencies/Libraries/glm-0.9.9.8/cmake/glm")
# set(ENV{VULKAN_SDK} "D:\\Applications\\Vulkan")


# libs
find_package(Vulkan REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(glm REQUIRED)


target_include_directories(${PROJECT_NAME} 
PUBLIC 
${Vulkan_INCLUDE_DIR} 
${GLFW_INCLUDE_DIRS} 
${GLM_INCLUDE_DIRS} 
)

target_link_libraries(${PROJECT_NAME} 
${Vulkan_LIBRARIES}
${GLFW_LIBRARIES}
)

