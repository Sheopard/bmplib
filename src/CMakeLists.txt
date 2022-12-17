cmake_minimum_required(VERSION 3.22)

project(bmplib VERSION 0.0.1 LANGUAGES CXX)

set(SOURCES
  bmp.cpp
)

set(HEADERS
  bmp.h
  bmp_exceptions.h
)

add_library(library STATIC ${SOURCES} ${HEADERS})

set_target_properties(
  library PROPERTIES
  CXX_STANDARD 14
  CXX_STANDARD_REQUIRED ON
)
