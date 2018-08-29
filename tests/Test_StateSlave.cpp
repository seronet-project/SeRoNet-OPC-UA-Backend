///
/// \file StateSlaveInstantiate.cpp
/// \author Christian von Arnim
/// \date 03.08.2018
///

#include <gtest/gtest.h>

#include <SeRoNet/State/StateSlave.hpp>
#include <SeRoNet/Utils/Component.hpp>

TEST(StateSlave, CreateDestroy) {
  SeRoNet::Utils::Component comp("Test_StateSlave");
  auto stateSlave = std::make_shared<SeRoNet::State::StateSlave>(&comp);
  stateSlave = nullptr;
  // Should not throw any exceptions/ SegFaults, ... (if so test will fail)
  EXPECT_TRUE(true);
}

TEST(StateSlave, AddStates) {
  SeRoNet::Utils::Component comp("Test_StateSlave");
  auto stateSlave = std::make_shared<SeRoNet::State::StateSlave>(&comp);
  std::string subStateName = "TestSubState";

  EXPECT_EQ(stateSlave->defineStates("TestState", subStateName), Smart::StatusCode::SMART_OK);
  EXPECT_EQ(stateSlave->defineStates("TestState", subStateName), Smart::StatusCode::SMART_OK);
  EXPECT_EQ(stateSlave->defineStates("TestState", subStateName), Smart::StatusCode::SMART_OK);

  stateSlave = nullptr;
}

TEST(StateSlave, acquire)
{
  SeRoNet::Utils::Component comp("Test_StateSlave");
  std::string subStateName = "TestSubState";
  std::string mainStateName = "TestState";
  auto stateSlave = std::make_shared<SeRoNet::State::StateSlave>(&comp);
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

