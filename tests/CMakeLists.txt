set(TENT_CASES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cases")
set(TENT_RUNNER "${CMAKE_CURRENT_SOURCE_DIR}/run_tent_case.cmake")

file(GLOB TENT_CASE_NAMES RELATIVE "${TENT_CASES_DIR}" "${TENT_CASES_DIR}/*")
list(SORT TENT_CASE_NAMES)

foreach(CASE_NAME ${TENT_CASE_NAMES})
    if(NOT IS_DIRECTORY "${TENT_CASES_DIR}/${CASE_NAME}")
        continue()
    endif()

    add_test(
        NAME tent.${CASE_NAME}
        COMMAND "${CMAKE_COMMAND}"
            "-DTENT_BIN=$<TARGET_FILE:tent>"
            "-DCASE_DIR=${TENT_CASES_DIR}/${CASE_NAME}"
            "-DWORK_DIR=${CMAKE_BINARY_DIR}"
            "-P" "${TENT_RUNNER}"
    )

    if(EXISTS "${TENT_CASES_DIR}/${CASE_NAME}/smoke")
        set_tests_properties(tent.${CASE_NAME} PROPERTIES LABELS "smoke")
    endif()
endforeach()
