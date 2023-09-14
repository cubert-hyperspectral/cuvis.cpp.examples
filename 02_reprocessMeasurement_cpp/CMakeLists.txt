cmake_minimum_required(VERSION 3.16.0)
project(02_reprocessMeasurement_cpp)
list(APPEND CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_LIST_DIR}/../cuvis.cpp/")
find_package(CuvisCpp 3.2 REQUIRED)
add_executable(02_reprocessMeasurement_cpp main.cpp)
target_link_libraries(02_reprocessMeasurement_cpp PRIVATE cuvis::cpp)