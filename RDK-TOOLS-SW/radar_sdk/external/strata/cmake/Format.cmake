macro(addFormatTargets)

    if(CLANG_FORMAT_PATH)
        set(CLANG_FORMAT_EXE ${CLANG_FORMAT_PATH}clang-format)
    else()
        # We have to use find_program at configuration time to use the correct path.
        # At runtime there could be an environment from Visual Studio, where a different clang-format
        # version is found, which will not work. For this reason the complete path is set here.
        find_program(CLANG_FORMAT_EXE "clang-format")
    endif()

    set(FILE_EXTENSIONS *.?pp *.h)
    set(CLANG_FORMAT_COMMAND_APPLY ${CLANG_FORMAT_EXE} --style=file --verbose -i)
    set(CLANG_FORMAT_COMMAND_CHECK ${CLANG_FORMAT_EXE} --style=file --dry-run --Werror)

    set(FORMAT_DIRS "")
    foreach(dir ${ARGN})
        file(GLOB_RECURSE SUB_DIRS
            RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
            LIST_DIRECTORIES True
            ${dir}/.
            )
        list(APPEND FORMAT_DIRS ${dir} ${SUB_DIRS})
    endforeach()

    set(FORMAT_TARGET_COMMANDS_APPLY "")
    set(FORMAT_TARGET_COMMANDS_CHECK "")
    foreach(dir ${FORMAT_DIRS})
        set(PARAMS "")
        foreach(ext ${FILE_EXTENSIONS})
            file(GLOB FILES ${dir}/${ext})
            if(FILES)
                # only add parameter if files are present, otherwise clang-format will output an error
                set(PARAMS ${PARAMS} ${dir}/${ext})
            endif()
        endforeach()
        if(PARAMS)
            set(FORMAT_TARGET_COMMANDS_APPLY ${FORMAT_TARGET_COMMANDS_APPLY} COMMAND ${CLANG_FORMAT_COMMAND_APPLY} ${PARAMS})
            set(FORMAT_TARGET_COMMANDS_CHECK ${FORMAT_TARGET_COMMANDS_CHECK} COMMAND ${CLANG_FORMAT_COMMAND_CHECK} ${PARAMS})
        endif()
    endforeach()

    add_custom_target(format_apply
        ${FORMAT_TARGET_COMMANDS_APPLY}
        SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/.clang-format"
        COMMENT "Apply clang-format to specified source and header files"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )

    add_custom_target(format_check
        ${FORMAT_TARGET_COMMANDS_CHECK}
        SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/.clang-format"
        COMMENT "Check clang-format of specified source and header files"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
endmacro()
