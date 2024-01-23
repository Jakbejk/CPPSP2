cmake_minimum_required(VERSION 3.27)
project(_SP2)

set(CMAKE_CXX_STANDARD 20)

add_executable(_SP2 main.cpp
        MpInt.h
        Test.h)
