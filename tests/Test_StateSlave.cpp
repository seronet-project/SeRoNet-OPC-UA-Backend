/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/


#include <gtest/gtest.h>

#include <SeRoNet/State/StateSlave.hpp>
#include <SeRoNet/Utils/Component.hpp>
#include "helperClasses/ComponentHelperTest.hpp"

class StateSlave : public ComponentHelperTest{

};

TEST_F(StateSlave, CreateDestroy) {
  auto stateSlave = std::make_shared<SeRoNet::State::StateSlave>(compServer);
  stateSlave = nullptr;
  // Should not throw any exceptions/ SegFaults, ... (if so test will fail)
  EXPECT_TRUE(true);
}

TEST_F(StateSlave, CreateDestroy2) {
  const int numRuns = 10;
  for(int i = 1; i <= numRuns; ++i)
  {
    std::cout << typeid(decltype(*this)).name() << ": Run " << i << " of " << numRuns << std::endl;
    auto stateSlave = std::make_shared<SeRoNet::State::StateSlave>(compServer);
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    stateSlave = nullptr;
  }
  // Should not throw any exceptions/ SegFaults, ... (if so test will fail)
  EXPECT_TRUE(true);
}

TEST_F(StateSlave, AddStates) {
  auto stateSlave = std::make_shared<SeRoNet::State::StateSlave>(compServer);
  std::string subStateName = "TestSubState";

  EXPECT_EQ(stateSlave->defineStates("TestState", subStateName), Smart::StatusCode::SMART_OK);
  EXPECT_EQ(stateSlave->defineStates("TestState", subStateName), Smart::StatusCode::SMART_OK);
  EXPECT_EQ(stateSlave->defineStates("TestState", subStateName), Smart::StatusCode::SMART_OK);

  stateSlave = nullptr;
}

TEST_F(StateSlave, acquire)
{
  std::string subStateName = "TestSubState";
  std::string mainStateName = "TestState";
  auto stateSlave = std::make_shared<SeRoNet::State::StateSlave>(compServer);
  EXPECT_EQ(stateSlave->defineStates(mainStateName, subStateName), Smart::StatusCode::SMART_OK);

  stateSlave->activate();
  std::thread myTh([&stateSlave](){
    std::this_thread::yield();
    std::this_thread::sleep_for(std::chrono::seconds(1));
  });

  //stateSlave->setWaitState(mainStateName);

  //EXPECT_EQ(stateSlave->acquire(subStateName), Smart::StatusCode::SMART_OK);
  EXPECT_EQ(stateSlave->tryAcquire(subStateName), Smart::StatusCode::SMART_NOTACTIVATED);
  EXPECT_EQ(stateSlave->release(subStateName), Smart::StatusCode::SMART_OK);
  EXPECT_EQ(stateSlave->release(subStateName), Smart::StatusCode::SMART_OK);

  EXPECT_TRUE(myTh.joinable());
  myTh.join();
  
  stateSlave = nullptr;
}

