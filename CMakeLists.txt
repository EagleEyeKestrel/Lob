cmake_minimum_required(VERSION 3.21)
project(Lob)

set(CMAKE_CXX_STANDARD 17)

add_executable(Lob main.cpp global.cpp fileWorker.cpp stockSim.cpp dateUtils.cpp)
