/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/
#include <SeRoNet/Utils/Component.hpp>
#include <thread>
#include <chrono>
#include <gtest/gtest.h>
#include <type_traits>

TEST(Component, RunAndKill)
{
  SeRoNet::Utils::Component *component = new SeRoNet::Utils::Component("Component_Tets");
  std::thread th(&std::remove_pointer<decltype(component)>::type::run, component );
  std::this_thread::sleep_for(std::chrono::seconds(2));
  component->stopRunning();
  th.join();
  EXPECT_TRUE(true);
}
