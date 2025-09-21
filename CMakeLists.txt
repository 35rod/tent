cmake_minimum_required(VERSION 4.1.1)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(newLang)

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
    src/vm.cpp
    src/parser.cpp
    src/errors.cpp
    src/token.cpp
    src/esc_codes.cpp
)

add_subdirectory(lib)

add_executable(main ${SOURCE_FILES})

if(MSVC)
	target_compile_options(main PRIVATE /W4 /WX)
else()
	target_compile_options(main PRIVATE -g -Wall -Wextra -Wpedantic)
endif()
