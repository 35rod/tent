find_package(FLTK REQUIRED)

file(GLOB LIB_SOURCES "*.cpp")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

foreach(SRC ${LIB_SOURCES})
	get_filename_component(LIB_NAME ${SRC} NAME_WE)
	add_library(${LIB_NAME} SHARED ${SRC})

	if(MSVC)
		target_compile_options(${LIB_NAME} PRIVATE /W4 /WX)
	else()
		target_compile_options(${LIB_NAME} PRIVATE -Wall -Wextra -Wpedantic -fPIC)
	endif()

	target_include_directories(${LIB_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include ${FLTK_INCLUDE_DIR})

	target_link_libraries(${LIB_NAME} PRIVATE ${FLTK_LIBRARIES})

	if(APPLE OR UNIX)
		set_target_properties(${LIB_NAME} PROPERTIES INSTALL_RPATH "${FLTK_LIBRARY_DIR}" BUILD_WITH_INSTALL_RPATH TRUE)
	endif()
endforeach()