﻿cmake_minimum_required (VERSION 3.28)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
	cmake_policy(SET CMP0141 NEW)
	set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("VT_Engine")
add_library(${PROJECT_NAME} SHARED)

# Project files
include(GenerateExportHeader)

generate_export_header(${PROJECT_NAME}
	EXPORT_FILE_NAME
		${CMAKE_CURRENT_SOURCE_DIR}/VT_Export
)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Files
target_sources(${PROJECT_NAME}
	PUBLIC
	FILE_SET CXX_MODULES
	FILES
		VT_Engine.cppm
		Engine.cppm
		Log.cppm
		Window.cppm
		Input.cppm
		Timestep.cppm
		ShaderCompiler.cppm
		File.cppm

	PRIVATE
		Engine.cpp
		Window.cpp
		Input.cpp
		ShaderCompiler.cpp
        Platform/Vulkan/Util.cppm
)

add_subdirectory(Event)
add_subdirectory(Layer)
add_subdirectory(Renderer)
add_subdirectory(Platform)
add_subdirectory(Util)

#Libs
add_subdirectory(Lib SYSTEM)

find_package(Vulkan REQUIRED 
	COMPONENTS
		shaderc_combined
		dxc
)
target_link_libraries(${PROJECT_NAME}
	PUBLIC
		Vulkan::Vulkan
		Vulkan::shaderc_combined
		Vulkan::dxc_lib
)

target_include_directories(${PROJECT_NAME}
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}
)

# Target properties
set_target_properties(${PROJECT_NAME}
	PROPERTIES
	LANGUAGES CXX
	CXX_STANDARD 20

	SOVERSION 0
	VERSION 0.1

	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
)
target_compile_definitions(${PROJECT_NAME}
	PRIVATE
		VT_ENABLE_MESSAGE
		VT_ENABLE_ASSERT
		VT_ENABLE_DEBUG
		
		GLFW_INCLUDE_NONE
		GLFW_INCLUDE_VULKAN
		
		GLM_FORCE_RADIANS
		GLM_FORCE_DEPTH_ZERO_TO_ONE

		# enable aggregate initialization
		VULKAN_HPP_NO_CONSTRUCTORS
		VULKAN_HPP_NO_EXCEPTIONS
		VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
)

# Compile flags
if (MSVC)
	# msvc is special like that
	target_compile_options(${PROJECT_NAME}
		PRIVATE
		/W4
		# ignore warn struct padding
		/wd4820
		# special member function implicit deleted
		/wd4623
		/wd4626
		/wd5027
		/wd5262
		/wd5267
		# /Wall
		# /WX
	)
	# CMake appends /W3 by default, and having /W3 followed by /W4 will result in
	# cl : Command line warning D9025 : overriding '/W3' with '/W4'.  Since this is
	# a command line warning and not a compiler warning, it cannot be suppressed except
	# by fixing the command line.
	string(REGEX REPLACE " /W[0-4]" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
	string(REGEX REPLACE " /W[0-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
else()
	# additional warnings
	target_compile_options(${PROJECT_NAME}
		PRIVATE
		-Wall
		-Wextra
		-Werror
		-Wno-error=unused-variable
		-Wpedantic
	)
endif()

# set(CMAKE_INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_DIR}/Include)
# set(CMAKE_INSTALL_LIB_DIR ${CMAKE_INSTALL_DIR}/Lib)
#
# install(TARGETS ${PROJECT_NAME}
# 	EXPORT ${PROJECT_NAME}-targets
# 	FILE_SET CXX_MODULES
# 		DESTINATION ${CMAKE_INSTALL_INCLUDE_DIR}
# 	FILE_SET HEADERS
# 		DESTINATION ${CMAKE_INSTALL_INCLUDE_DIR}
# 	INCLUDES
# 		DESTINATION ${CMAKE_INSTALL_INCLUDE_DIR}
# )
# install(EXPORT ${PROJECT_NAME}-targets
# 	DESTINATION ${CMAKE_INSTALL_LIB_DIR}/cmake/VT_Engine
# 	CXX_MODULES_DIRECTORY ${CMAKE_INCLUDE_DIR}
# )
#
# install(FILES ${PROJECT_NAME}-config.cmake
# 	DESTINATION ${CMAKE_INSTALL_LIB_DIR}/cmake/VT_Engine
# )
