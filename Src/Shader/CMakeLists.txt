# Build HLSL shaders
function(compile_shader TARGET)
    if(ARGC GREATER_EQUAL 2)
        math(EXPR argc_mod_two "${ARGC} % 2")
        math(EXPR last_idx "${ARGC} - 1")
        if(NOT argc_mod_two EQUAL "1")
            message(FATAL_ERROR "Invalid number of arguments passed to ${CMAKE_CURRENT_FUNCTION} - expected an even amount, got ${last_idx}")
        endif()

        find_program(DXC NAMES glslc PATHS ${VULKAN_SDK}/bin)

        if (${GLSLC})
            MESSAGE(STATUS "Found GLSLC.")
        else()
          MESSAGE(STATUS "Not Found GLSLC.")
        endif()

        # Add a custom command to compile GLSL to SPIR-V.
        file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/Shaders)

        foreach(IDX RANGE 1 "${last_idx}" 2)
            math(EXPR TYPE_IDX "${IDX} + 1")

            set(SHADER ${ARGV${IDX}})
            set(FILETYPE ${ARGV${IDX}})

            set(SHADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${SHADER})
            set(SHADER_OUTPUT_PATH ${PROJECT_BINARY_DIR}/Shader/${SHADER}.spv)

            add_custom_command(
            OUTPUT ${SHADER_OUTPUT_PATH}
            COMMAND ${DXC} -fshader-stage=${FILETYPE} ${SHADER_PATH} -o ${SHADER_OUTPUT_PATH} 
            DEPENDS ${SHADER_PATH}
            IMPLICIT_DEPENDS CXX ${SHADER_PATH}
            VERBATIM)

            # Make sure our build depends on this output.
            set_source_files_properties(${SHADER_OUTPUT_PATH} PROPERTIES GENERATED TRUE)
            target_sources(${TARGET} PRIVATE ${SHADER_OUTPUT_PATH})
        endforeach()
    endif()
endfunction(compile_shader)

function(t TARGET)
    # Add a custom command to compile GLSL to SPIR-V.
    file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/Shaders)

    foreach(SHADER ${ARGN})
         set(SHADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${SHADER})
            set(SHADER_OUTPUT_PATH ${PROJECT_BINARY_DIR}/Shader/${SHADER}.spv)

            add_custom_command(
            OUTPUT ${SHADER_OUTPUT_PATH}
            COMMAND dxc ${SHADER_PATH} -o ${SHADER_OUTPUT_PATH} 
            DEPENDS ${SHADER_PATH}
            IMPLICIT_DEPENDS CXX ${SHADER_PATH}
            VERBATIM)

            # Make sure our build depends on this output.
            set_source_files_properties(${SHADER_OUTPUT_PATH} PROPERTIES GENERATED TRUE)
            target_sources(${TARGET} PRIVATE ${SHADER_OUTPUT_PATH})
    endforeach()
endfunction(t)
t(${PROJECT_NAME}
"VertexShader.hlsl"
"FragmentShader.hlsl"
)