///
/// \file Test_Event.cpp
/// \author Christian von Arnim
/// \date 06.09.2018
///

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <thread>
#include <SeRoNet/CommunicationObjects/Description/SelfDescriptionPrimitives.hpp>

#include <SeRoNet/OPCUA/Client/EventClient.hpp>
#include <SeRoNet/OPCUA/Server/EventServer.hpp>

//                                                ActivationType EventType UpdateType
class MockIEventTestHandler : public Smart::IEventTestHandler<int, int, int> {
 public:
  MOCK_METHOD3_T(testEvent, bool(int & , int & ,
      const int&));
};

TEST(Event, Activate) {
  std::string serverCompName("Test_Event_Activate_Server");
  std::string serviceName("ActivateTest");
  SeRoNet::Utils::Component serverComp(serverCompName);

  MockIEventTestHandler evTestHandler;

  /// T_ParameterType T_ResultType T_StatusType T_IdentificationType
  SeRoNet::OPCUA::Server::EventServer<int, int, int, int> evServer(&serverComp, serviceName, &evTestHandler);
  std::thread serverRun([&serverComp]() { serverComp.run(); });
  std::this_thread::sleep_for(std::chrono::seconds(1));

  EXPECT_CALL(evTestHandler, testEvent(::testing::_, ::testing::_, ::testing::_)).Times(::testing::Exactly(0));

  std::string clientCompName("Test_Event_Activate_Client");
  SeRoNet::Utils::Component clientComp(clientCompName);
  SeRoNet::OPCUA::Client::EventClient<int, int> eventClient(&clientComp);

  EXPECT_EQ(eventClient.connect(serverCompName, serviceName), Smart::StatusCode::SMART_OK);
  decltype(eventClient)::EventIdType id;
  EXPECT_EQ(eventClient.activate(Smart::EventMode::continuous, 1, id), Smart::StatusCode::SMART_OK);
  id = decltype(id)();
  SeRoNet::OPCUA::Server::OpcUaServer::instance().stopRunning();
  serverRun.join();
}

TEST(Event, TryEvent) {
  std::string serverCompName("Test_Event_TryEvent_Server");
  std::string serviceName("TryEventTest");
  SeRoNet::Utils::Component serverComp(serverCompName);

  MockIEventTestHandler evTestHandler;

  /// T_ParameterType T_ResultType T_StatusType T_IdentificationType
  SeRoNet::OPCUA::Server::EventServer<int, int, int, int> evServer(&serverComp, serviceName, &evTestHandler);
  std::thread serverRun([&serverComp]() { serverComp.run(); });
  std::this_thread::sleep_for(std::chrono::seconds(1));

  std::string clientCompName("Test_Event_TryEvent_Client");
  SeRoNet::Utils::Component clientComp(clientCompName);
  SeRoNet::OPCUA::Client::EventClient<int, int> eventClient(&clientComp);

  EXPECT_EQ(eventClient.connect(serverCompName, serviceName), Smart::StatusCode::SMART_OK);
  decltype(eventClient)::EventIdType id;
  EXPECT_EQ(eventClient.activate(Smart::EventMode::continuous, 1, id), Smart::StatusCode::SMART_OK);

  EXPECT_EQ(eventClient.tryEvent(id), Smart::StatusCode::SMART_ERROR);

  EXPECT_CALL(evTestHandler, testEvent(::testing::_, ::testing::_, ::testing::_))
      .Times(::testing::Exactly(1))
      .WillRepeatedly(::testing::Return(true));

  EXPECT_NO_THROW(evServer.put(3));

  EXPECT_EQ(eventClient.tryEvent(id), Smart::StatusCode::SMART_OK);

  id = decltype(id)();
  SeRoNet::OPCUA::Server::OpcUaServer::instance().stopRunning();
  serverRun.join();
  EXPECT_TRUE(true);
}