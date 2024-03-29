macro(git_submodule_build _project_name)
    set(options INSTALL_SUB_DEPS)
    set(oneValueArgs "")
    set(multiValueArgs CMAKE_ARGS)
    cmake_parse_arguments(${_project_name} "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    message("-- Build submodule '${_project_name}' at ${CMAKE_SOURCE_DIR}/remote/${_project_name}")

    set(${_project_name}_command
            -G ${CMAKE_GENERATOR}
            .
            -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/fakeroot
            -B${CMAKE_BINARY_DIR}/remote/${_project_name}
            )

    list(APPEND ${_project_name}_command ${${_project_name}_CMAKE_ARGS})

    execute_process(COMMAND ${CMAKE_COMMAND} ${${_project_name}_command}
            RESULT_VARIABLE result
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/remote/${_project_name}
            )

    if(result)
        message(FATAL_ERROR "CMake step for ${_project_name} failed: ${result}")
    endif()

    execute_process(COMMAND ${CMAKE_COMMAND} --build . --target install
            RESULT_VARIABLE result
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/remote/${_project_name}
            )

    if(result)
        message(FATAL_ERROR "Build step for ${_project_name} failed: ${result}")
    endif()

    if (${_project_name}_INSTALL_SUB_DEPS)
        message("Install sub-dependenies of ${_project_name}...")

        file(COPY "${CMAKE_BINARY_DIR}/remote/${_project_name}/fakeroot/" DESTINATION "${CMAKE_BINARY_DIR}/fakeroot/")
    endif()

endmacro()


macro(git_submodule_copy_files _project_name)
    set(options NO_NAME_INCLUDE)
    set(oneValueArgs EXPLICIT_INCLUDE_NAME EXPLICIT_INCLUDE_DIR)
    set(multiValueArgs INCLUDES LIBRARIES)
    cmake_parse_arguments(${_project_name} "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (${_project_name}_NO_NAME_INCLUDE AND DEFINED ${_project_name}_EXPLICIT_INCLUDE_NAME)
        message(FATAL_ERROR "Can't use NO_NAME_INCLUDE with EXPLICIT_INCLUDE_NAME")
    endif()

    if (${_project_name}_NO_NAME_INCLUDE AND DEFINED ${_project_name}_EXPLICIT_INCLUDE_DIR)
        message(FATAL_ERROR "Can't use NO_NAME_INCLUDE with EXPLICIT_INCLUDE_DIR")
    endif()

    if (DEFINED ${_project_name}_EXPLICIT_INCLUDE_DIR AND ${_project_name}_EXPLICIT_INCLUDE_NAME)
        message(FATAL_ERROR "Can't use EXPLICIT_INCLUDE_DIR with EXPLICIT_INCLUDE_NAME")
    endif()

    foreach(_file ${${_project_name}_INCLUDES})
        get_filename_component(_path ${_file} DIRECTORY)
        set(_src_path "${CMAKE_SOURCE_DIR}/remote/${_project_name}/${_file}")
        set(_dst_path "${CMAKE_BINARY_DIR}/fakeroot/include")

        if(${_project_name}_NO_NAME_INCLUDE)
            file(COPY "${_src_path}" DESTINATION "${_dst_path}/${_path}")
        else()
            if (DEFINED ${_project_name}_EXPLICIT_INCLUDE_DIR)
                file(COPY "${_src_path}" DESTINATION "${_dst_path}/${${_project_name}_EXPLICIT_INCLUDE_DIR}")
            elseif (DEFINED ${_project_name}_EXPLICIT_INCLUDE_NAME)
                file(COPY "${_src_path}" DESTINATION "${_dst_path}/${${_project_name}_EXPLICIT_INCLUDE_NAME}/${_path}")
            else()
                file(COPY "${_src_path}" DESTINATION "${_dst_path}/${_project_name}/${_path}")
            endif()
        endif()
    endforeach()

    foreach(_file ${${_project_name}_LIBRARIES})
        get_filename_component(_path ${_file} DIRECTORY)
        file(COPY "${CMAKE_SOURCE_DIR}/remote/${_project_name}/${_file}" DESTINATION "${CMAKE_BINARY_DIR}/fakeroot/lib/${_path}")
    endforeach()
endmacro()