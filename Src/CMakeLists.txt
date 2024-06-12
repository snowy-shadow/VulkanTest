cmake_minimum_required (VERSION 3.28)

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME}
	PRIVATE
		App.cpp
)

# find_package(VT_Engine REQUIRED)
target_link_libraries(${PROJECT_NAME}
	PRIVATE
		VT_Engine
)

set_target_properties(${PROJECT_NAME}
	PROPERTIES
	CXX_STANDARD 20

	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
)

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
		/Wall
		/WX
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
		-O2
	)
endif()
