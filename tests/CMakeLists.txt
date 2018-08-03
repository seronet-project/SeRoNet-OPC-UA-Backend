cmake_minimum_required(VERSION 3.0)
project(SeRoNetSDK_Tests)

set(CMAKE_CXX_STANDARD 14)

add_executable(Test_StateSlaveInstantiate Test_StateSlave.cpp)
target_link_libraries(Test_StateSlaveInstantiate SeRoNetSDK gtest_main)
