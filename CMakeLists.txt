# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("VulkanTest" VERSION 0.1 LANGUAGES CXX)

# Custom libraries
if(APPLE)
	set(GLFW_PATH "/opt/homebrew")
elseif(WIN32)
	set(GLFW_PATH "S:/Dev/Dependencies/Libraries/glfw/glfw-3.3.9")
endif()

if(MSVC)
	# CMake appends /W3 by default, and having /W3 followed by /W4 will result in
	# cl : Command line warning D9025 : overriding '/W3' with '/W4'.  Since this is
	# a command line warning and not a compiler warning, it cannot be suppressed except
	# by fixing the command line.
	string(REGEX REPLACE " /W[0-4]" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
	string(REGEX REPLACE " /W[0-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeFind")

# clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(VulkanEngine)
add_subdirectory (Src)
