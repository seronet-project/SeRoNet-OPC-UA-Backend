/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/


#include <gtest/gtest.h>
#include <SeRoNet/State/StateUpdateThread.hpp>
#include <SeRoNet/State/StateSlave.hpp>
#include <type_traits>
#include "../helperClasses/ComponentHelperTest.hpp"

class StateUpdateThread : public ComponentHelperTest {

};

TEST_F(StateUpdateThread, initShutdown) {
  const int numRuns = 10;

  SeRoNet::State::StateSlave stateSlave(compClient);

  for (int i = 1; i <= numRuns; ++i) {
    std::cout << typeid(decltype(*this)).name() << ": Run " << i << " of " << numRuns << std::endl;

    auto pInstance = std::make_shared<SeRoNet::State::StateUpdateThread>(compClient);
    pInstance->init(&stateSlave);
    pInstance->start();
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    pInstance->stop(true);
    pInstance = nullptr;
  }

  EXPECT_TRUE(true);
}
