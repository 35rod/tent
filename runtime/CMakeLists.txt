set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/runtime)

add_library(TentRuntime STATIC
	box_ops.cpp
	dynamic_ops.cpp
)

target_include_directories(TentRuntime PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
)
