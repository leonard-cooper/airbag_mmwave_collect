
function(strata_add_unit_test name)
    add_executable(${name} ${name}.cpp)
    add_test(NAME ${name} COMMAND ${name} --gtest_output=xml:UnitTest${name}.xml WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} )
    set_tests_properties(${name} PROPERTIES LABELS "UNIT")
    target_link_libraries(${name} gmock_main ${ARGN})
    target_include_directories(${name} PUBLIC "${STRATA_DIR}/tests")
endfunction()

function(strata_add_unit_test_with_input_files)
    set(options OPTIONAL)
    set(oneValueArgs TEST_NAME)
    set(multiValueArgs INPUT_FILES TEST_LIBRARIES)
    cmake_parse_arguments(STRATA "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    strata_add_unit_test(${STRATA_TEST_NAME} ${STRATA_TEST_LIBRARIES})
    #parsing optional parameters in order to copy the input test files to working directory
    #copy each input file to output directory
    foreach(arg IN LISTS STRATA_INPUT_FILES)
        add_custom_command(TARGET ${STRATA_TEST_NAME} POST_BUILD  COMMAND ${CMAKE_COMMAND} -E copy  ${arg}  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    endforeach()
endfunction()

function(strata_add_integration_test name)
    add_executable(${name} ${name}.cpp ${ARGN} $<TARGET_OBJECTS:CatchMain>)
    add_test(NAME ${name} COMMAND ${name})
    set_tests_properties(${name} PROPERTIES LABELS "INTEGRATION")
    target_include_directories_from_object(${name} catch2)
    target_link_libraries(${name} ${STRATA_TEST_LINK_LIBRARIES})
    target_include_directories(${name} PUBLIC "${STRATA_DIR}/tests")
endfunction()

function(strata_add_python_test name type binary_path)
    add_test(NAME ${name}
        COMMAND ${PYTHON_EXECUTABLE} -m unittest discover -s "${CMAKE_CURRENT_SOURCE_DIR}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
    set_tests_properties(${name} PROPERTIES LABELS ${type})

    list(APPEND binary_path $ENV{PYTHONPATH})
    if(UNIX)
        string(REPLACE ";" ":" binary_path "${binary_path}")
    endif()
    set_tests_properties(${name} PROPERTIES ENVIRONMENT "PYTHONPATH=${binary_path}")
endfunction()
