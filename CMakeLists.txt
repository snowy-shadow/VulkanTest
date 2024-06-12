# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.28)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("VulkanTest" VERSION 0.1 LANGUAGES CXX)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# Custom libraries
if(APPLE)
  set(GLFW_PATH "/opt/homebrew")
elseif(WIN32)
  set(GLFW_PATH "S:/Dev/Dependencies/Libraries/glfw/glfw-3.3.9")
endif()

if(WIN32)
  list(APPEND CMAKE_PREFIX_PATH "S:/Dev/Dependencies/Libraries")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

# clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_INSTALL_DIR S:/Dev/Dependencies/Libraries/VT_Engine)
# C++20 std modules
# set(VCTOOLS_INSTALL_DIR "S:/Dev/IDE/Visual Studio/VS/VC/Tools/MSVC/14.40.33807")
# include(${CMAKE_SOURCE_DIR}/CMake/EnableStandardLibraryModule.cmake)

add_subdirectory(Engine)
add_subdirectory(Src)
