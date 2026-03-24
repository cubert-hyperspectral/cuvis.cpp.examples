cmake_minimum_required(VERSION 3.16.0)
project(Example_4_Exporters_cpp)
list(APPEND CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_LIST_DIR}/../cuvis.cpp/")
find_package(CuvisCpp 3.2 REQUIRED)
add_executable(Example_4_Exporters_cpp main.cpp)
target_link_libraries(Example_4_Exporters_cpp PRIVATE cuvis::cpp)