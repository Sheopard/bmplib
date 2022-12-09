cmake_minimum_required(VERSION 3.22)

project(bmplib)

set(SOURCES
	src/bmp.cpp
)

set(HEADERS
	src/bmp.h
	src/bmp_exceptions.h	
)


add_executable(bmp.exe main.cpp ${SOURCES} ${HEADERS})

set_target_properties(
	bmp.exe PROPERTIES
	CXX_STANDARD 14
	CXX_STANDARD_REQUIRED ON
)

