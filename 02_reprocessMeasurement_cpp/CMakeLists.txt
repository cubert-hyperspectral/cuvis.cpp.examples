cmake_minimum_required(VERSION 3.20.0)
project(Example_02)
list(APPEND CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_LIST_DIR}/../cuvis.cpp/")
find_package(CuvisCpp 3.2 REQUIRED)
add_executable(Example_02 main.cpp)
target_link_libraries(Example_02 PRIVATE cuvis::cpp)