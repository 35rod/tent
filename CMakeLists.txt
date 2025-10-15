cmake_minimum_required(VERSION 4.1.1)
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
    src/vm.cpp
    src/parser.cpp
    src/errors.cpp
    src/token.cpp
    src/esc_codes.cpp
)

add_subdirectory(lib)

add_executable(tent ${SOURCE_FILES})

if(MSVC)
	target_compile_options(tent PRIVATE -O3 /W4 /WX)
else()
	target_compile_options(tent PRIVATE -O3 -Wall -Wextra -Wpedantic)
endif()
