# Groups Engine modules into single library
cmake_minimum_required (VERSION 3.8)

project ("VulkanEngine" VERSION 0.1 LANGUAGES CXX)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# custom libs
add_subdirectory(Util)
add_subdirectory(Device)
add_subdirectory(Renderer)

add_library(${PROJECT_NAME} INTERFACE)
target_link_libraries(${PROJECT_NAME}
	INTERFACE
	Util
    Renderer
    Device
)
