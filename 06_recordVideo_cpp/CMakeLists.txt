cmake_minimum_required(VERSION 3.16.0)
project(06_recordVideo_cpp)
list(APPEND CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_LIST_DIR}/../cuvis.cpp/")
find_package(CuvisCpp 3.2 REQUIRED)
add_executable(06_recordVideo_cpp main.cpp)
target_link_libraries(06_recordVideo_cpp PRIVATE cuvis::cpp)