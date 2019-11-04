/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/


#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <thread>
#include <SeRoNet/CommunicationObjects/Description/SelfDescriptionPrimitives.hpp>

#include <SeRoNet/OPCUA/Client/EventClient.hpp>
#include <SeRoNet/OPCUA/Server/EventServer.hpp>

#include "helperClasses/ComponentHelperTest.hpp"

//                                                ActivationType EventType UpdateType
class MockIEventTestHandler : public Smart::IEventTestHandler<int, int, int> {
 public:
  MOCK_METHOD3_T(testEvent, bool(int & , int & ,
      const int&));
};

class Event : public ComponentHelperTest {

};

TEST_F(Event, Activate) {
  std::string serviceName("ActivateTest");

  MockIEventTestHandler evTestHandler;

  /// T_ParameterType T_ResultType T_StatusType T_IdentificationType
  SeRoNet::OPCUA::Server::EventServer<int, int, int, int> evServer(compServer, serviceName, &evTestHandler);

  SeRoNet::OPCUA::Client::EventClient<int, int> eventClient(compServer);

  EXPECT_CALL(evTestHandler, testEvent(::testing::_, ::testing::_, ::testing::_)).Times(::testing::Exactly(0));

  startServer();

  EXPECT_EQ(eventClient.connect(compServer->getName(), serviceName), Smart::StatusCode::SMART_OK);
  decltype(eventClient)::EventIdType id;
  EXPECT_EQ(eventClient.activate(Smart::EventMode::continuous, 1, id), Smart::StatusCode::SMART_OK);
  id = decltype(id)();
}

TEST_F(Event, TryEvent) {

  std::string serviceName("TryEventTest");

  MockIEventTestHandler evTestHandler;

  /// T_ParameterType T_ResultType T_StatusType T_IdentificationType
  SeRoNet::OPCUA::Server::EventServer<int, int, int, int> evServer(compServer, serviceName, &evTestHandler);

  SeRoNet::OPCUA::Client::EventClient<int, int> eventClient(compServer);

  startServer();

  EXPECT_EQ(eventClient.connect(compServer->getName(), serviceName), Smart::StatusCode::SMART_OK);
  decltype(eventClient)::EventIdType id;
  EXPECT_EQ(eventClient.activate(Smart::EventMode::continuous, 1, id), Smart::StatusCode::SMART_OK);

  EXPECT_EQ(eventClient.tryEvent(id), Smart::StatusCode::SMART_ERROR);

  EXPECT_CALL(evTestHandler, testEvent(::testing::_, ::testing::_, ::testing::_))
      .Times(::testing::Exactly(1))
      .WillRepeatedly(::testing::Return(true));

  int value = 3;
  EXPECT_NO_THROW(evServer.put(value));

  // After 1s the result must be availiable
  std::this_thread::sleep_for(std::chrono::seconds(1));

  EXPECT_EQ(eventClient.tryEvent(id), Smart::StatusCode::SMART_OK);

  int result = 0;
  EXPECT_EQ(eventClient.getEvent(id,result, std::chrono::seconds(1)), Smart::StatusCode::SMART_OK);

  EXPECT_EQ(result, value);

}