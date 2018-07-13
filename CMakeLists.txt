cmake_minimum_required(VERSION 3.10)
project(rozvrh)

set(CMAKE_CXX_STANDARD 11)

add_executable(rozvrh main.cpp timetable.cpp timetable.h config.h)