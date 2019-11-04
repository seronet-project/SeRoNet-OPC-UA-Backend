/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#include <gtest/gtest.h>
#include <SeRoNet/State/StateMaster.hpp>
#include <SeRoNet/State/StateSlave.hpp>
#include <SeRoNet/Utils/Component.hpp>
#include "helperClasses/ComponentHelperTest.hpp"

class State : public ComponentHelperTest{

};

TEST_F(State, MasterCreateDestroy) {
  auto stateMaster = std::make_shared<SeRoNet::State::StateMaster>(compServer);
  stateMaster = nullptr;
  EXPECT_TRUE(true);
}

TEST_F(State, QueryStates) {

  auto stateSlave = std::make_shared<SeRoNet::State::StateSlave>(compServer);
  EXPECT_EQ(Smart::StatusCode::SMART_OK, stateSlave->defineStates("TestState", "TestSubState"));

  stateSlave->activate();

  auto stateMaster = std::make_shared<SeRoNet::State::StateMaster>(compServer);

  startServer();

  std::string retMainState;
  EXPECT_EQ(Smart::StatusCode::SMART_OK, stateMaster->getCurrentMainState(retMainState, compServer->getName()));
  EXPECT_EQ("Init", retMainState);

  stateSlave = nullptr;

}

TEST_F(State, Acquire) {
  auto stateSlave = std::make_shared<SeRoNet::State::StateSlave>(compServer);
  std::string subStateName = "TestSubState";
  std::string mainStateName = "TestState";
  EXPECT_EQ(Smart::StatusCode::SMART_OK, stateSlave->defineStates("TestState", subStateName));

  stateSlave->activate();
  stateSlave->setWaitState("Alive");
  auto stateMaster = std::make_shared<SeRoNet::State::StateMaster>(compServer);

  startServer();

  EXPECT_EQ(Smart::StatusCode::SMART_OK, stateMaster->setWaitState(mainStateName, compServer->getName()));

  EXPECT_EQ(stateSlave->tryAcquire(subStateName), Smart::StatusCode::SMART_OK);
  EXPECT_EQ(stateSlave->release(subStateName), Smart::StatusCode::SMART_OK);

  stateSlave = nullptr;

}
