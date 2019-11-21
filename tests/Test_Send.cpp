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
#include <SeRoNet/Utils/Component.hpp>

#include <SeRoNet/CommunicationObjects/Description/SelfDescriptionPrimitives.hpp>
#include <SeRoNet/CommunicationObjects/Description/SelfDescriptionArray.hpp>

#include <vector>
#include <SeRoNet/CommunicationObjects/Description/ElementArray.hpp>

#include <SeRoNet/OPCUA/Client/SendClient.hpp>
#include <SeRoNet/OPCUA/Server/SendServerHandler.hpp>
#include <SeRoNet/OPCUA/Server/SendServer.hpp>

#include "helperClasses/ComponentHelperTest.hpp"
#include "Mocks/MockSendHandler.hpp"

class Send : public ComponentHelperTest
{

};

TEST_F(Send, SendPrimitive) {
  /// ------------ Setup Server ------------
  std::string serviceName("SendPrimitive");
  auto qHandler = std::make_shared<MockSendHandler<int>>();
  SeRoNet::OPCUA::Server::SendServer<int> SendServer(compServer, serviceName, qHandler);

  /// ------------ Setup Client ------------
  SeRoNet::OPCUA::Client::SendClient<int> SendClient(compServer);
  startServer();

  /// ------------ Setup Mocks ------------
  int clientReq = 1;
  EXPECT_CALL(*qHandler, handleSend(clientReq))
      .Times(::testing::Exactly(1));


  EXPECT_EQ(Smart::StatusCode::SMART_OK, SendClient.connect(compServer->getName(), serviceName));

  /// ------------ Communicate ------------
  EXPECT_EQ(Smart::StatusCode::SMART_OK, SendClient.send(clientReq));
}


TEST_F(Send, SendPrimitiveArray) {
  typedef std::vector<int> ioType_t;
  /// ------------ Setup Server ------------
  std::string serviceName("SendPrimitiveArray");
  auto qHandler = std::make_shared<MockSendHandler<ioType_t>>();
  SeRoNet::OPCUA::Server::SendServer<ioType_t> SendServer(compServer, serviceName, qHandler);

  /// ------------ Setup Client ------------
  SeRoNet::OPCUA::Client::SendClient<ioType_t> SendClient(compServer);
  startServer();

  /// ------------ Setup Mocks ------------
  const ioType_t req = {1, 2, 3};
  EXPECT_CALL(*qHandler, handleSend(req))
      .Times(::testing::Exactly(1));

  EXPECT_EQ(Smart::StatusCode::SMART_OK, SendClient.connect(compServer->getName(), serviceName));

  /// ------------ Communicate ------------
  EXPECT_EQ(Smart::StatusCode::SMART_OK, SendClient.send(req));
}
