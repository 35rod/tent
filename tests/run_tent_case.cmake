cmake_minimum_required(VERSION 3.10)

if(NOT DEFINED TENT_BIN)
    message(FATAL_ERROR "TENT_BIN is required")
endif()

if(NOT DEFINED CASE_DIR)
    message(FATAL_ERROR "CASE_DIR is required")
endif()

if(NOT DEFINED WORK_DIR)
    set(WORK_DIR "${CMAKE_BINARY_DIR}")
endif()

set(PROGRAM_FILE "${CASE_DIR}/program.tent")
set(ARGS_FILE "${CASE_DIR}/args.txt")
set(STDIN_FILE "${CASE_DIR}/stdin.txt")
set(EXPECTED_OUT_FILE "${CASE_DIR}/expected.out")
set(EXPECTED_ERR_FILE "${CASE_DIR}/expected.err")
set(EXPECTED_ERR_CONTAINS_FILE "${CASE_DIR}/expected.err.contains")
set(EXPECTED_CODE_FILE "${CASE_DIR}/expected.code")

if(NOT EXISTS "${PROGRAM_FILE}")
    message(FATAL_ERROR "missing test program: ${PROGRAM_FILE}")
endif()

set(CASE_ARGS)
if(EXISTS "${ARGS_FILE}")
    file(STRINGS "${ARGS_FILE}" CASE_ARGS)
endif()

set(COMMAND_ARGS "${PROGRAM_FILE}")
if(CASE_ARGS)
    list(APPEND COMMAND_ARGS -- ${CASE_ARGS})
endif()

if(EXISTS "${STDIN_FILE}")
    execute_process(
        COMMAND "${TENT_BIN}" ${COMMAND_ARGS}
        WORKING_DIRECTORY "${WORK_DIR}"
        INPUT_FILE "${STDIN_FILE}"
        RESULT_VARIABLE ACTUAL_CODE
        OUTPUT_VARIABLE ACTUAL_OUT
        ERROR_VARIABLE ACTUAL_ERR
    )
else()
    execute_process(
        COMMAND "${TENT_BIN}" ${COMMAND_ARGS}
        WORKING_DIRECTORY "${WORK_DIR}"
        RESULT_VARIABLE ACTUAL_CODE
        OUTPUT_VARIABLE ACTUAL_OUT
        ERROR_VARIABLE ACTUAL_ERR
    )
endif()

set(EXPECTED_OUT "")
if(EXISTS "${EXPECTED_OUT_FILE}")
    file(READ "${EXPECTED_OUT_FILE}" EXPECTED_OUT)
endif()

set(HAS_EXPECTED_ERR FALSE)
set(EXPECTED_ERR "")
if(EXISTS "${EXPECTED_ERR_FILE}")
    set(HAS_EXPECTED_ERR TRUE)
    file(READ "${EXPECTED_ERR_FILE}" EXPECTED_ERR)
endif()

set(EXPECTED_CODE "0")
if(EXISTS "${EXPECTED_CODE_FILE}")
    file(READ "${EXPECTED_CODE_FILE}" EXPECTED_CODE)
endif()
string(STRIP "${EXPECTED_CODE}" EXPECTED_CODE)

string(REPLACE "\r\n" "\n" ACTUAL_OUT "${ACTUAL_OUT}")
string(REPLACE "\r" "\n" ACTUAL_OUT "${ACTUAL_OUT}")
string(REPLACE "\r\n" "\n" EXPECTED_OUT "${EXPECTED_OUT}")
string(REPLACE "\r" "\n" EXPECTED_OUT "${EXPECTED_OUT}")
string(REGEX REPLACE "\n+$" "" ACTUAL_OUT "${ACTUAL_OUT}")
string(REGEX REPLACE "\n+$" "" EXPECTED_OUT "${EXPECTED_OUT}")

string(REPLACE "\r\n" "\n" ACTUAL_ERR "${ACTUAL_ERR}")
string(REPLACE "\r" "\n" ACTUAL_ERR "${ACTUAL_ERR}")
string(REPLACE "\r\n" "\n" EXPECTED_ERR "${EXPECTED_ERR}")
string(REPLACE "\r" "\n" EXPECTED_ERR "${EXPECTED_ERR}")
string(REGEX REPLACE "\n+$" "" ACTUAL_ERR "${ACTUAL_ERR}")
string(REGEX REPLACE "\n+$" "" EXPECTED_ERR "${EXPECTED_ERR}")

if("${EXPECTED_CODE}" STREQUAL "nonzero")
    if("${ACTUAL_CODE}" STREQUAL "0")
        message(FATAL_ERROR
            "case '${CASE_DIR}' expected non-zero exit code, but got 0\n"
            "stderr:\n${ACTUAL_ERR}"
        )
    endif()
elseif(NOT "${ACTUAL_CODE}" STREQUAL "${EXPECTED_CODE}")
    message(FATAL_ERROR
        "case '${CASE_DIR}' exit code mismatch\n"
        "expected: ${EXPECTED_CODE}\n"
        "actual:   ${ACTUAL_CODE}\n"
        "stderr:\n${ACTUAL_ERR}"
    )
endif()

if(NOT "${ACTUAL_OUT}" STREQUAL "${EXPECTED_OUT}")
    message(FATAL_ERROR
        "case '${CASE_DIR}' stdout mismatch\n"
        "expected:\n<<<\n${EXPECTED_OUT}\n>>>\n"
        "actual:\n<<<\n${ACTUAL_OUT}\n>>>"
    )
endif()

if(HAS_EXPECTED_ERR)
    if(NOT "${ACTUAL_ERR}" STREQUAL "${EXPECTED_ERR}")
        message(FATAL_ERROR
            "case '${CASE_DIR}' stderr mismatch\n"
            "expected:\n<<<\n${EXPECTED_ERR}\n>>>\n"
            "actual:\n<<<\n${ACTUAL_ERR}\n>>>"
        )
    endif()
endif()

if(EXISTS "${EXPECTED_ERR_CONTAINS_FILE}")
    file(STRINGS "${EXPECTED_ERR_CONTAINS_FILE}" EXPECTED_ERR_SUBSTRINGS)
    foreach(SNIPPET ${EXPECTED_ERR_SUBSTRINGS})
        if(SNIPPET STREQUAL "")
            continue()
        endif()

        string(FIND "${ACTUAL_ERR}" "${SNIPPET}" MATCH_IDX)
        if(MATCH_IDX EQUAL -1)
            message(FATAL_ERROR
                "case '${CASE_DIR}' stderr did not contain required text\n"
                "missing: '${SNIPPET}'\n"
                "actual stderr:\n${ACTUAL_ERR}"
            )
        endif()
    endforeach()
endif()
