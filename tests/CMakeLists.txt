cmake_minimum_required(VERSION 3.0)
project(SeRoNetSDK_Tests)

set(CMAKE_CXX_STANDARD 14)

# NO_CMAKE_SYSTEM_PATH prevents the usage of 'FindGTest.cmake' from the cmake
# installation. As this does not include GMock.
find_package(GTest REQUIRED NO_CMAKE_SYSTEM_PATH)

add_library(CommObjs
		CommObjs/CommScanOpcUa.cpp
		CommObjs/CommPose3dOpcUa.cpp
		)
target_link_libraries(CommObjs SeRoNetSDK)

add_executable(Test_StateSlave Test_StateSlave.cpp)
target_link_libraries(Test_StateSlave SeRoNetSDK GTest::gtest_main)
add_test(NAME StateSlave COMMAND Test_StateSlave)
set_tests_properties(StateSlave PROPERTIES TIMEOUT 20)

add_executable(Test_Query Test_Query.cpp)
target_link_libraries(Test_Query SeRoNetSDK GTest::gmock_main)
add_test(NAME Query COMMAND Test_Query)
set_tests_properties(Query PROPERTIES TIMEOUT 60)

add_executable(Test_Send Test_Send.cpp)
target_link_libraries(Test_Send SeRoNetSDK GTest::gmock_main)
add_test(NAME Send COMMAND Test_Send)
set_tests_properties(Send PROPERTIES TIMEOUT 60)

add_executable(Test_Push Test_Push.cpp)
target_link_libraries(Test_Push SeRoNetSDK GTest::gmock_main CommObjs)
add_test(NAME Push COMMAND Test_Push)
set_tests_properties(Push PROPERTIES TIMEOUT 600)

add_executable(Test_Converter Test_Converter.cpp)
target_link_libraries(Test_Converter SeRoNetSDK GTest::gtest_main)
add_test(NAME Converter COMMAND Test_Converter)
set_tests_properties(Converter PROPERTIES TIMEOUT 10)

add_executable(Test_State Test_State.cpp)
target_link_libraries(Test_State SeRoNetSDK GTest::gtest_main)
add_test(NAME State COMMAND Test_State)
set_tests_properties(State PROPERTIES TIMEOUT 60)

add_executable(Test_Event Test_Event.cpp)
target_link_libraries(Test_Event SeRoNetSDK GTest::gmock_main)
add_test(NAME Event COMMAND Test_Event)
set_tests_properties(Event PROPERTIES TIMEOUT 60)

add_executable(Test_PushSend Test_PushSend.cpp)
target_link_libraries(Test_PushSend SeRoNetSDK GTest::gmock_main)
add_test(NAME PushSend COMMAND Test_PushSend)
set_tests_properties(Event PROPERTIES TIMEOUT 60)

# All testcases in one executeable, which is not added to ctest!
FILE(GLOB_RECURSE all_test_src ./*.cpp)
add_executable(Test_all ${all_test_src})
target_link_libraries(Test_all SeRoNetSDK GTest::gmock_main)

add_subdirectory(internalClasses)
