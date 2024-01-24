cmake_minimum_required(VERSION 3.26)
project(Calculator)

set(CMAKE_CXX_STANDARD 20)

add_executable(Calculator main.cpp
        MpInt.h
        Test.h
        MpTerm.h)
