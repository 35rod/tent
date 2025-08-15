cmake_minimum_required(VERSION 4.1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(newLang)

include_directories(include)

set(SOURCE_FILES
    src/main.cpp
    src/ast.cpp
    src/lexer.cpp
    src/evaluator.cpp
    src/parser.cpp
    src/token.cpp
)

add_executable(main ${SOURCE_FILES})