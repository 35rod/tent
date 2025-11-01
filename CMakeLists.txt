cmake_minimum_required(VERSION 3.10.2)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(tent)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

llvm_map_components_to_libnames(LLVM_LIBS
    Core
    Support
    IRReader
    ExecutionEngine
    Target
)

include_directories(include)

set(SOURCE_FILES
    src/main.cpp
    src/types.cpp
    src/native.cpp
    src/ast.cpp
    src/args.cpp
    src/lexer.cpp
    src/evaluator.cpp
    src/parser.cpp
    src/errors.cpp
    src/token.cpp
    src/esc_codes.cpp
)

add_subdirectory(lib)

add_executable(tent ${SOURCE_FILES})

target_link_libraries(tent PRIVATE ${LLVM_LIBS})

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
