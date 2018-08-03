///
/// \file StateSlaveInstantiate.cpp
/// \author Christian von Arnim
/// \date 03.08.2018
///

#include <gtest/gtest.h>

#include <SeRoNet/State/StateSlave.hpp>
#include <SeRoNet/Utils/Component.hpp>

TEST(StateSlave, CreateDestroy)
{
  SeRoNet::Utils::Component comp("Test_StateSlave");
  auto stateSlave = std::make_shared<SeRoNet::State::StateSlave>(&comp);
  stateSlave = nullptr;
  // Should not throw any exceptions/ SegFaults, ... (if so test failes)
  EXPECT_TRUE(true);
}
