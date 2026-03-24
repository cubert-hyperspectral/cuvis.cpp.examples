cmake_minimum_required(VERSION 3.16.0)
project(Example_1_Take_Snapshot_cpp)
list(APPEND CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_LIST_DIR}/../cuvis.cpp/")
find_package(CuvisCpp 3.2 REQUIRED)
add_executable(Example_1_Take_Snapshot_cpp main.cpp)
target_link_libraries(Example_1_Take_Snapshot_cpp PRIVATE cuvis::cpp)