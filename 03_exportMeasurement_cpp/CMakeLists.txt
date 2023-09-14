cmake_minimum_required(VERSION 3.20.0)
project(03_exportMeasurement_cpp)
list(APPEND CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_LIST_DIR}/../cuvis.cpp/")
find_package(CuvisCpp 3.2 REQUIRED)
add_executable(03_exportMeasurement_cpp main.cpp)
target_link_libraries(03_exportMeasurement_cpp PRIVATE cuvis::cpp)