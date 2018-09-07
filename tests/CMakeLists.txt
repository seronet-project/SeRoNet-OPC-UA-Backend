cmake_minimum_required(VERSION 3.0)
project(SeRoNetSDK_Tests)

set(CMAKE_CXX_STANDARD 14)

add_executable(StateSlave Test_StateSlave.cpp)
target_link_libraries(StateSlave SeRoNetSDK gtest_main)
add_test(StateSlave StateSlave)

add_executable(Test_Query Test_Query.cpp)
target_link_libraries(Test_Query SeRoNetSDK gtest gmock_main)
add_test(Query Test_Query)

add_executable(Test_Push Test_Push.cpp)
target_link_libraries(Test_Push SeRoNetSDK gtest gmock_main)
add_test(Push Test_Push)


add_executable(Test_Converter Test_Converter.cpp)
target_link_libraries(Test_Converter SeRoNetSDK gtest_main)
add_test(Converter Test_Converter)

add_executable(Test_State Test_State.cpp)
target_link_libraries(Test_State SeRoNetSDK gtest_main)
add_test(State Test_State)

add_executable(Test_StateSlave Test_StateSlave.cpp)
target_link_libraries(Test_StateSlave SeRoNetSDK gtest_main)
add_test(StateSlave Test_StateSlave)

add_executable(Test_Event Test_Event.cpp)
target_link_libraries(Test_Event SeRoNetSDK gtest gmock_main)
add_test(Event Test_Event)

