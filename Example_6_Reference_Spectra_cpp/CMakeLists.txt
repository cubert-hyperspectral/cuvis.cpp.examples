cmake_minimum_required(VERSION 3.16.0)
project(Example_6_Reference_Spectra_cpp)
list(APPEND CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_LIST_DIR}/../cuvis.cpp/")
find_package(CuvisCpp 3.6 REQUIRED)
add_executable(Example_6_Reference_Spectra_cpp main.cpp)
target_link_libraries(Example_6_Reference_Spectra_cpp PRIVATE cuvis::cpp)