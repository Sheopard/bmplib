cmake_minimum_required(VERSION 3.22)

project(main_example VERSION 0.0.1 LANGUAGES CXX)

add_executable(bmp.exe main.cpp)

set_target_properties(
  bmp.exe PROPERTIES
  CXX_STANDARD 14
  CXX_STANDARD_REQUIRED ON
)

target_include_directories(
  bmp.exe PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

target_link_libraries(bmp.exe library)
