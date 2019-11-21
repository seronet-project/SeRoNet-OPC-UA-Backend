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
#include <SeRoNet/CommunicationObjects/Description/IDescription.hpp>
#include <SeRoNet/CommunicationObjects/Description/ComplexType.hpp>
#include <SeRoNet/CommunicationObjects/Description/IDescription.hpp>
#include <SeRoNet/CommunicationObjects/Description/ElementPrimitives.hpp>

#include <SeRoNet/OPCUA/Client/PushClient.hpp>
#include <SeRoNet/OPCUA/Server/PushServer.hpp>

#include "helperClasses/ComponentHelperTest.hpp"

#include "CommObjs/CoordinateObject.hpp"

#include "CommObjs/CommScanOpcUa.h"

class Push : public ComponentHelperTest {

};

template<typename T_COMMOBJECT>
class PushTemplate : public Push {

};

typedef ::testing::Types<bool,
                         std::uint8_t,
                         std::uint16_t,
                         std::uint32_t,
                         std::int8_t,
                         std::int16_t,
                         std::int32_t,
                         float,
                         double> PushTestTypes;
TYPED_TEST_CASE(PushTemplate, PushTestTypes);

TYPED_TEST(PushTemplate, GetUpdate) {

  /// ------------ Setup Client ------------
  SeRoNet::OPCUA::Client::PushClient<TypeParam> pushClient(this->compServer);

  /// ------------ Setup Server ------------
  std::string serviceName("ComplexDataTypeGetUpdate");
  SeRoNet::OPCUA::Server::PushServer<TypeParam> pushServer(this->compServer, serviceName);
  ASSERT_EQ(this->compServer->getOpcUaServer()->getServerName(), this->compServer->getName());

  this->startServer();

  /// ---- connect Client -----
  ASSERT_EQ(Smart::StatusCode::SMART_OK, pushClient.connect(this->compServer->getName(), serviceName));
  ASSERT_EQ(Smart::StatusCode::SMART_OK, pushClient.subscribe(1));
  /// ------------ Communicate ------------

  //FIXME create a Factory for Values
  TypeParam clientElement = 42;
  TypeParam serverElement = 33;

  ASSERT_EQ(Smart::StatusCode::SMART_OK, pushServer.put(serverElement));
  std::this_thread::sleep_for(std::chrono::milliseconds(500)); // ensure that element has been read
  ASSERT_EQ(Smart::StatusCode::SMART_OK, pushClient.getUpdate(clientElement));
  ASSERT_EQ(serverElement, clientElement);

}

TEST_F(Push, ComplexDataTypeGetUpdate) {

  /// ------------ Setup Client ------------
  SeRoNet::OPCUA::Client::PushClient<CoordinateObject> pushClient(compServer);

  /// ------------ Setup Server ------------
  std::string serviceName("ComplexDataTypeGetUpdate");
  SeRoNet::OPCUA::Server::PushServer<CoordinateObject> pushServer(compServer, serviceName);
  ASSERT_EQ(compServer->getOpcUaServer()->getServerName(), compServer->getName());

  startServer();

  /// ---- connect Client -----
  ASSERT_EQ(Smart::StatusCode::SMART_OK, pushClient.connect(compServer->getName(), serviceName));
  ASSERT_EQ(Smart::StatusCode::SMART_OK, pushClient.subscribe(1));
  /// ------------ Communicate ------------

  CoordinateObject clientElement = {4, 5, 6};
  CoordinateObject serverElement = {1, 2, 3};

  ASSERT_EQ(Smart::StatusCode::SMART_OK, pushServer.put(serverElement));
  std::this_thread::sleep_for(std::chrono::milliseconds(500)); // ensure that element has been read
  ASSERT_EQ(Smart::StatusCode::SMART_OK, pushClient.getUpdate(clientElement));
  ASSERT_EQ(serverElement, clientElement);

  EXPECT_TRUE(true);
}

TEST_F(Push, ComplexDataTypeGetUpdateWait) {

  /// ------------ Setup Client ------------
  SeRoNet::OPCUA::Client::PushClient<CoordinateObject> pushClient(compServer);

  /// ------------ Setup Server ------------
  std::string serviceName("ComplexDataTypeGetUpdateWait");
  SeRoNet::OPCUA::Server::PushServer<CoordinateObject> pushServer(compServer, serviceName);
  ASSERT_EQ(compServer->getOpcUaServer()->getServerName(), compServer->getName());

  startServer();

  /// ---- connect Client -----
  ASSERT_EQ(Smart::StatusCode::SMART_OK, pushClient.connect(compServer->getName(), serviceName));
  ASSERT_EQ(Smart::StatusCode::SMART_OK, pushClient.subscribe(1));
  /// ------------ Communicate ------------

  CoordinateObject clientElement = {4, 5, 6};

  CoordinateObject serverElement = {1, 2, 3};
  Smart::StatusCode retVal = Smart::StatusCode::SMART_PASSIVE;
  std::thread comp_client_thread([&]() { retVal = pushClient.getUpdateWait(clientElement); });

  std::this_thread::sleep_for(std::chrono::seconds(1));
  ASSERT_EQ(Smart::StatusCode::SMART_OK, pushServer.put(serverElement));
  comp_client_thread.join();
  ASSERT_EQ(Smart::StatusCode::SMART_OK, retVal);
  ASSERT_EQ(serverElement, clientElement);

}

TEST_F(Push, SameDataTwice) {

  /// ------------ Setup Client ------------
  SeRoNet::OPCUA::Client::PushClient<int> pushClient(compServer);

  /// ------------ Setup Server ------------
  std::string serviceName(test_info_->name());
  SeRoNet::OPCUA::Server::PushServer<int> pushServer(compServer, serviceName);
  ASSERT_EQ(compServer->getOpcUaServer()->getServerName(), compServer->getName());

  startServer();

  /// ---- connect Client -----
  ASSERT_EQ(Smart::StatusCode::SMART_OK, pushClient.connect(compServer->getName(), serviceName));
  ASSERT_EQ(Smart::StatusCode::SMART_OK, pushClient.subscribe(1));
  /// ------------ Communicate ------------

  int clientElement = 4;
  int serverElement = 3;

  Smart::StatusCode retVal = Smart::StatusCode::SMART_PASSIVE;
  std::thread comp_client_thread([&]() {
    retVal = pushClient.getUpdateWait(clientElement);
    retVal = pushClient.getUpdateWait(clientElement);
  });

  std::this_thread::sleep_for(std::chrono::seconds(1));
  ASSERT_EQ(Smart::StatusCode::SMART_OK, pushServer.put(serverElement));
  std::this_thread::sleep_for(std::chrono::seconds(1));
  ASSERT_EQ(Smart::StatusCode::SMART_OK, pushServer.put(serverElement));
  comp_client_thread.join();
  ASSERT_EQ(Smart::StatusCode::SMART_OK, retVal);
  ASSERT_EQ(serverElement, clientElement);
}

TEST_F(Push, MultiPush) {

  /// ------------ Setup Client ------------
  SeRoNet::OPCUA::Client::PushClient<int> pushClient(compServer);

  /// ------------ Setup Server ------------
  std::string serviceName(test_info_->name());
  SeRoNet::OPCUA::Server::PushServer<int> pushServer(compServer, serviceName);
  ASSERT_EQ(compServer->getOpcUaServer()->getServerName(), compServer->getName());

  startServer();

  /// ---- connect Client -----
  ASSERT_EQ(Smart::StatusCode::SMART_OK, pushClient.connect(compServer->getName(), serviceName));
  ASSERT_EQ(Smart::StatusCode::SMART_OK, pushClient.subscribe(1));
  /// ------------ Communicate ------------

  std::this_thread::sleep_for(std::chrono::seconds(1));
  for (int i = 100; i > 0; --i) {
    ASSERT_EQ(Smart::StatusCode::SMART_OK, pushServer.put(i));
    std::cout << "i: " << i << std::endl;
  }

}

TEST_F(Push, ArrayComplex) {

  /// ------------ Setup Client ------------
  SeRoNet::OPCUA::Client::PushClient<CommScan> pushClient(compServer);

  /// ------------ Setup Server ------------
  std::string serviceName(test_info_->name());
  SeRoNet::OPCUA::Server::PushServer<CommScan> pushServer(compServer, serviceName);
  ASSERT_EQ(compServer->getOpcUaServer()->getServerName(), compServer->getName());

  startServer();

  /// ---- connect Client -----
  ASSERT_EQ(Smart::StatusCode::SMART_OK, pushClient.connect(compServer->getName(), serviceName));
  ASSERT_EQ(Smart::StatusCode::SMART_OK, pushClient.subscribe(1));
  /// ------------ Communicate ------------

  CommScan serverElement = {
      {1, 2, 3},
      {1.1, 2.2, 3.3},
      {
          {1.2, 2.3, 3.4},
          {2.2, 3.3, 4.4},
          {3.2, 4.3, 5.4},
      }
  };

  CommScan clientElement;

  Smart::StatusCode retVal = Smart::StatusCode::SMART_PASSIVE;
  std::thread comp_client_thread([&]() { retVal = pushClient.getUpdateWait(clientElement); });

  ASSERT_EQ(Smart::StatusCode::SMART_OK, pushServer.put(serverElement));
  comp_client_thread.join();
  ASSERT_EQ(Smart::StatusCode::SMART_OK, retVal);
  ASSERT_EQ(serverElement, clientElement);
}
