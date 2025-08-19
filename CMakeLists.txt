cmake_minimum_required(VERSION 4.1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(newLang)

include_directories(include ext_include)

set(SOURCE_FILES
    src/main.cpp
    src/ast.cpp
    src/args.cpp
    src/lexer.cpp
    src/evaluator.cpp
    src/parser.cpp
    src/errors.cpp
    src/token.cpp
)

add_executable(main ${SOURCE_FILES} ext_obj/util-log.o)

if(MSVC)
	target_compile_options(main PRIVATE /W4 /WX)
else()
	target_compile_options(main PRIVATE -Wall -Wextra -Wpedantic)
endif()
