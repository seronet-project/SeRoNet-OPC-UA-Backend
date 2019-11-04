/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#pragma once

#include <gtest/gtest.h>
#include <SeRoNet/Utils/Component.hpp>
#include <thread>

class ComponentHelperTest : public ::testing::Test
{
 protected:
  inline void SetUp() override {
    ::testing::Test::SetUp();
    {
      std::stringstream ss;
      ss << "Test_" << typeid(*this).name() << "_Server";
      compServer = new SeRoNet::Utils::Component(ss.str());
    }

    {
      std::stringstream ss;
      ss << "Test_" << typeid(*this).name() << "_Client";
      compClient = new SeRoNet::Utils::Component(ss.str());
    }
  }

  inline void TearDown() override {
    ::testing::Test::TearDown();
    compServer->stopRunning();
    compClient->stopRunning();
    if(comp_server_thread.joinable())
    {
      comp_server_thread.join();
    }
    if(comp_client_thread.joinable())
    {
      comp_client_thread.join();
    }
    delete compServer;
    delete compClient;
  }

  inline void startServer()
  {
    /// Move semantics  with '{}' !
    comp_server_thread = std::thread{&SeRoNet::Utils::Component::run, compServer};
    comp_client_thread = std::thread{&SeRoNet::Utils::Component::run, compClient};
  }

  SeRoNet::Utils::Component *compServer = nullptr;
  SeRoNet::Utils::Component *compClient = nullptr;
  std::thread comp_server_thread;
  std::thread comp_client_thread;
};
