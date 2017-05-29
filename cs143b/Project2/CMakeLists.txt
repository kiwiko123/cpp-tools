cmake_minimum_required(VERSION 3.6)
project(Project2)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.cpp scheduling_algorithm.hpp scheduling_algorithm.cpp fifo_algorithm.hpp fifo_algorithm.cpp algorithm_exception.hpp algorithm_exception.cpp process.hpp process.cpp scheduler.hpp scheduler.cpp sjf_algorithm.hpp sjf_algorithm.cpp srt_algorithm.hpp srt_algorithm.cpp mlf_queue.hpp mlf_algorithm.hpp mlf_algorithm.cpp)
add_executable(Project2 ${SOURCE_FILES})