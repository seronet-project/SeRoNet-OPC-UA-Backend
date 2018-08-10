///
/// \file Test_State.cpp
/// \author Christian von Arnim
/// \date 10.08.2018
///

#include <gtest/gtest.h>
#include <SeRoNet/State/StateMaster.hpp>
#include <SeRoNet/State/StateSlave.hpp>
#include <SeRoNet/Utils/Component.hpp>

TEST(State, MasterCreateDestroy) {
  SeRoNet::Utils::Component comp("Test_StateMaster");
  auto stateMaster = std::make_shared<SeRoNet::State::StateMaster>(&comp);
  stateMaster = nullptr;
  EXPECT_TRUE(true);
}

TEST(State, QueryStates) {
  SeRoNet::Utils::Component compSlave("Test_StateSlave");
  auto stateSlave = std::make_shared<SeRoNet::State::StateSlave>(&compSlave);
  EXPECT_EQ(Smart::StatusCode::SMART_OK, stateSlave->defineStates("TestState", "TestSubState"));

  stateSlave->activate();

  SeRoNet::Utils::Component compMaster("Test_StateMaster");
  auto stateMaster = std::make_shared<SeRoNet::State::StateMaster>(&compMaster);

  std::thread comp_server_thread(&decltype(compSlave)::run, &compSlave);

  std::string retMainState;
  EXPECT_EQ(Smart::StatusCode::SMART_OK, stateMaster->getCurrentMainState(retMainState, compSlave.getName()));
  EXPECT_EQ("Init", retMainState);

  stateSlave = nullptr;
  compSlave.stopRunning();
  comp_server_thread.join();

}
