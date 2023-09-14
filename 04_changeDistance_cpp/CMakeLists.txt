cmake_minimum_required(VERSION 3.16.0)
project(04_changeDistance_cpp)
list(APPEND CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_LIST_DIR}/../cuvis.cpp/")
find_package(CuvisCpp 3.2 REQUIRED)
add_executable(04_changeDistance_cpp main.cpp)
target_link_libraries(04_changeDistance_cpp PRIVATE cuvis::cpp)