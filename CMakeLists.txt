cmake_minimum_required(VERSION 3.10.2)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(tent)

include_directories(include)

set(SOURCE_FILES
    src/main.cpp
    src/types.cpp
    src/native.cpp
    src/ast.cpp
    src/args.cpp
    src/lexer.cpp
    src/evaluator.cpp
    src/compiler.cpp
    src/parser.cpp
    src/diagnostics.cpp
    src/errors.cpp
    src/token.cpp
    src/esc_codes.cpp
)

add_subdirectory(runtime)
add_subdirectory(lib)

add_executable(tent ${SOURCE_FILES})

# this chunk of cmake script primarily courtesy of GPT-5 mini, unfortunately.
find_package(LLVM CONFIG)
if(LLVM_FOUND AND TARGET LLVM::Core)
    target_link_libraries(tent PRIVATE
        LLVM::Core
        LLVM::Support
        LLVM::IRReader
        LLVM::Analysis
        LLVM::Passes
        TentRuntime
    )

    message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
    message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

    include_directories(${LLVM_INCLUDE_DIRS})
    #add_definitions(${LLVM_DEFINITIONS})
else()
    find_program(LLVM_CONFIG_EXECUTABLE llvm-config)
    if(LLVM_CONFIG_EXECUTABLE)
        execute_process(COMMAND
            ${LLVM_CONFIG_EXECUTABLE} --ldflags --libs core support irreader analysis passes transformutils instrumentation profiledata bitreader bitwriter object support
            OUTPUT_VARIABLE LLVM_LIBS_RAW
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        separate_arguments(LLVM_LIBS_RAW)
        string(REPLACE "\n" " " LLVM_LIBS_RAW "${LLVM_LIBS_RAW}")
        target_link_libraries(tent PRIVATE ${LLVM_LIBS_RAW})
        message(STATUS "Found LLVM via llvm-config with link flags: ${LLVM_LIBS_RAW}")

        execute_process(COMMAND ${LLVM_CONFIG_EXECUTABLE} --includedir
            OUTPUT_VARIABLE LLVM_INCLUDE_DIRS_RAW
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        include_directories(${LLVM_INCLUDE_DIRS_RAW})
    else()
        message(FATAL_ERROR "LLVM not found: set LLVM_DIR or LLVM_CONFIG_EXECUTABLE")
    endif()
endif()
# end of stuff written by GPT-5 mini

if(MSVC)
    target_compile_options(tent PRIVATE -O3 /W4 /WX)
else()
    target_compile_options(tent PRIVATE -O3 -Wall -Wextra -Wpedantic)
endif()

install(
    TARGETS tent
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION bin
)

install(
    DIRECTORY ${CMAKE_SOURCE_DIR}/lib/
    DESTINATION lib/tent
    FILES_MATCHING
    PATTERN "*.dll"
)

install(
    DIRECTORY ${CMAKE_SOURCE_DIR}/lib/
    DESTINATION lib/tent
    FILES_MATCHING
    PATTERN "*.so"
)

install(
    DIRECTORY ${CMAKE_SOURCE_DIR}/lib/
    DESTINATION lib/tent
    FILES_MATCHING
    PATTERN "*.dylib"
)

install(
    FILES ${CMAKE_SOURCE_DIR}/runtime/libTentRuntime.a
    DESTINATION lib/tent
)

include(CPack)

set(CPACK_PACKAGE_NAME "tent")
set(CPACK_PACKAGE_VENDOR "35rod")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Tent Programming Language")
set(CPACK_PACKAGE_VERSION "0.1.0")

if(WIN32)
    set(CPACK_GENERATOR "NSIS")
elseif(APPLE)
    set(CPACK_GENERATOR "TGZ")
else()
    set(CPACK_GENERATOR "TGZ;DEB")
endif()

set(CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_BINARY_DIR};${PROJECT_NAME};ALL;/")
