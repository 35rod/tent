set(BENCH_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/fib.tent"
    "${CMAKE_CURRENT_SOURCE_DIR}/loop.tent"
    "${CMAKE_CURRENT_SOURCE_DIR}/sieve.tent"
    "${CMAKE_CURRENT_SOURCE_DIR}/str_concat.tent"
)

set(BENCH_ITERATIONS "5" CACHE STRING "Number of iterations per benchmark")

add_executable(bench_runner "${CMAKE_CURRENT_SOURCE_DIR}/runner.cpp")
target_compile_features(bench_runner PRIVATE cxx_std_17)

if(MSVC)
    target_compile_options(bench_runner PRIVATE /W4)
else()
    target_compile_options(bench_runner PRIVATE -Wall -Wextra)
endif()

add_custom_target(bench
    COMMAND $<TARGET_FILE:bench_runner>
            $<TARGET_FILE:tent>
            ${BENCH_ITERATIONS}
            ${BENCH_FILES}
    DEPENDS bench_runner tent
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    COMMAND_EXPAND_LISTS
    VERBATIM
)
