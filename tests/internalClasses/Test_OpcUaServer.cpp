/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/


#include <SeRoNet/OPCUA/Server/OpcuaServer.hpp>
#include <gtest/gtest.h>
#include <type_traits>
#include <thread>
#include <chrono>

TEST(OpcUaServer, initAndRun)
{
  SeRoNet::OPCUA::Server::OpcUaServer opcuaServer;
  opcuaServer.initServer("MyName");
  std::thread th(&decltype(opcuaServer)::run, &opcuaServer);
  std::this_thread::sleep_for(std::chrono::seconds(5));
  opcuaServer.stopRunning();
  th.join();

  EXPECT_TRUE(true);
}
