find_path(GLFW_INCLUDE_DIRS GLFW\\glfw3.h
        PATH_SUFFIXES "include"
        PATHS ${GLFW_PATH})

find_library(GLFW_LIBRARIES NAMES glfw3
        PATH_SUFFIXES "lib-vc2022"
        PATHS ${GLFW_PATH})