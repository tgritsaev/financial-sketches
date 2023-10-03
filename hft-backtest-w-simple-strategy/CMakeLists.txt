cmake_minimum_required(VERSION 3.17)
project(A_cpp)

set(CMAKE_CXX_STANDARD 20)

add_executable(A_cpp A.cpp
        backtest.cpp
        backtest.h
        struct_request.cpp
        struct_request.h
        test.h
        deal.cpp
        strategy_mean_reversion.cpp)