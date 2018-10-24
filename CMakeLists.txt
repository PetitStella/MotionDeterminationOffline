cmake_minimum_required(VERSION 3.12)
project(MotionDeterminationOffline)

set(CMAKE_CXX_STANDARD 14)


set(SOURCE_FILES
        main.cpp
        ./INCLUDE/R-LlgmnClass.cpp
        ./INCLUDE/Common.cpp
        )

include_directories(/INCLUDE)
add_executable(MotionDeterminationOffline ${SOURCE_FILES})