#!/usr/bin/env sh
rm -rf build/CMakeCache.txt build/CMakeFiles build/cmake_install.cmake
if [ "$1" = "all" ]; then
	rm -rf build/*
fi
