cmake_minimum_required(VERSION 3.6)
project(Project3)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.cpp vm_system.hpp vm_system.cpp memory_exception.hpp memory_exception.cpp bit_map.hpp bit_map.cpp virtual_address.hpp virtual_address.cpp tlb.hpp tlb.cpp)
add_executable(Project3 ${SOURCE_FILES})