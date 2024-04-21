find_path(GLFW_INCLUDE_DIRS 
		GLFW/glfw3.h
		PATH_SUFFIXES "include"
        PATHS ${GLFW_PATH})

if(MSVC)
    find_library(GLFW_LIBRARY
		NAMES glfw3
        PATH_SUFFIXES "lib-vc2022"
        PATHS ${GLFW_PATH}) 
else()
    find_library(GLFW_LIBRARY
		NAMES glfw
        PATHS ${GLFW_PATH}) 
endif()
