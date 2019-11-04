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
#include <SeRoNet/OPCUA/Server/PushServer.hpp>
#include <SeRoNet/OPCUA/Client/NamingServiceOpcUa.hpp>

#include "../helperClasses/ComponentHelperTest.hpp"

#include <thread>
#include <chrono>

class NamingService : public ComponentHelperTest {

};

TEST_F(NamingService, connect) {
  //TODO Remove next two lines??
  std::string serviceName("PushPrimitive");
  SeRoNet::OPCUA::Server::PushServer<int> pushServer(compServer, serviceName);

  startServer();
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  ASSERT_EQ(compServer->getOpcUaServer()->isRunning(), true);

  auto namingService = std::make_shared<SeRoNet::OPCUA::Client::NamingServiceOpcUa>(compServer->getOpcUaServer());
  auto connAndNodeId = namingService->getConnectionAndNodeIdByName(compServer->getName(), serviceName);
  ASSERT_NE(nullptr, connAndNodeId.connection);

}

TEST_F(NamingService, replaceName)
{
  {
    startServer();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    ASSERT_EQ(compServer->getOpcUaServer()->isRunning(), true);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
  TearDown();
  SetUp();
  {

    std::string serviceName(this->test_info_->name());
    SeRoNet::OPCUA::Server::PushServer<int> pushServer(compServer, serviceName);

    startServer();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    ASSERT_EQ(compServer->getOpcUaServer()->isRunning(), true);

    auto namingService = std::make_shared<SeRoNet::OPCUA::Client::NamingServiceOpcUa>(compServer->getOpcUaServer());
    auto connAndNodeId = namingService->getConnectionAndNodeIdByName(compServer->getName(), serviceName);
    ASSERT_NE(nullptr, connAndNodeId.connection);
  }

}
