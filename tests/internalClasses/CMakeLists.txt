cmake_minimum_required(VERSION 3.0)

# Contains tests for internal classes (when the public API is not testable)

add_executable(Test_NamingService Test_NamingService.cpp)
target_link_libraries(Test_NamingService SeRoNetSDK GTest::gmock_main)
add_test(NAME NamingService COMMAND Test_NamingService)
set_tests_properties(NamingService PROPERTIES TIMEOUT 60)

add_executable(Test_Component Test_Component.cpp)
target_link_libraries(Test_Component SeRoNetSDK GTest::gmock_main)
add_test(NAME Component COMMAND Test_Component)
set_tests_properties(Component PROPERTIES TIMEOUT 30)

add_executable(Test_OpcUaServer Test_OpcUaServer.cpp)
target_link_libraries(Test_OpcUaServer SeRoNetSDK GTest::gmock_main)
add_test(NAME OpcUaServer COMMAND Test_OpcUaServer)
set_tests_properties(OpcUaServer PROPERTIES TIMEOUT 30)

# '_' prevents issue, that exe-Files with admin rights require Admin-Rights
add_executable(Test_StateUp_dateThread Test_StateUpdateThread.cpp)
target_link_libraries(Test_StateUp_dateThread SeRoNetSDK GTest::gmock_main)
add_test(NAME StateUpdateThread COMMAND Test_StateUp_dateThread)
set_tests_properties(StateUpdateThread PROPERTIES TIMEOUT 30)
