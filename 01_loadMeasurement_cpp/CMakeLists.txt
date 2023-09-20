cmake_minimum_required(VERSION 3.16.0)
project(01_loadMeasurement_cpp)
list(APPEND CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_LIST_DIR}/../cuvis.cpp/")
find_package(CuvisCpp 3.2 REQUIRED)
add_executable(01_loadMeasurement_cpp main.cpp)
target_link_libraries(01_loadMeasurement_cpp PRIVATE cuvis::cpp)