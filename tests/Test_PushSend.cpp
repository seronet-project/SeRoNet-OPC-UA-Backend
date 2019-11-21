/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#include <gtest/gtest.h>
#include <SeRoNet/CommunicationObjects/Description/SelfDescriptionPrimitives.hpp>
#include "helperClasses/ComponentHelperTest.hpp"
#include <SeRoNet/OPCUA/Client/PushClient.hpp>
#include <SeRoNet/OPCUA/Server/PushServer.hpp>

#include <SeRoNet/OPCUA/Client/SendClient.hpp>
#include <SeRoNet/OPCUA/Server/SendServer.hpp>
#include "Mocks/MockSendHandler.hpp"

class PushSend : public ComponentHelperTest
{

};


TEST_F(PushSend, SimpleObj){
  /// ------------ Setup Client ------------
  SeRoNet::OPCUA::Client::PushClient<int> pushClient(this->compServer);
  SeRoNet::OPCUA::Client::SendClient<int> sendClient(this->compServer);

  /// ------------ Setup Server ------------
  std::string serviceNamePush("pushServiceTest");
  std::string serviceNameSend("sendServiceTest");
  SeRoNet::OPCUA::Server::PushServer<int> pushServer(this->compServer, serviceNamePush);
  auto qHandler = std::make_shared<MockSendHandler<int>>();
  SeRoNet::OPCUA::Server::SendServer<int> sendServer(this->compServer, serviceNameSend, qHandler);
  ASSERT_EQ(this->compServer->getOpcUaServer()->getServerName(), this->compServer->getName());


  this->startServer();

  /// ---- connect Client -----
  ASSERT_EQ(Smart::StatusCode::SMART_OK, pushClient.connect(this->compServer->getName(), serviceNamePush));
  ASSERT_EQ(Smart::StatusCode::SMART_OK, sendClient.connect(this->compServer->getName(), serviceNameSend));
  ASSERT_EQ(Smart::StatusCode::SMART_OK, pushClient.subscribe(1));
  /// ------------ Communicate ------------

  //FIXME create a Factory for Values
  int clientElement = 42;
  int serverElement = 33;
  EXPECT_CALL(*qHandler, handleSend(clientElement))
      .Times(::testing::Exactly(1));


  ASSERT_EQ(Smart::StatusCode::SMART_OK, pushServer.put(serverElement));
  ASSERT_EQ(Smart::StatusCode::SMART_OK, sendClient.send(clientElement));
  std::this_thread::sleep_for(std::chrono::milliseconds(500)); // ensure that element has been read
  ASSERT_EQ(Smart::StatusCode::SMART_OK, pushClient.getUpdate(clientElement));

  ASSERT_EQ(serverElement, clientElement);
};
