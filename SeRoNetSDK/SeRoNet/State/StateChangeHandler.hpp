/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#pragma once
#include <string>

namespace SeRoNet {
namespace State {

/** Handler class for state changes at the slave
   *
   *  Used by StateSlave to notify the user about
   *  state changes.
   */
class StateChangeHandler {
 public:
  virtual ~StateChangeHandler() {};

  // Handler methods
  /** Called when a substate is entered
   *
   *  @param SubState name of newly entered substate
   */
  virtual void handleEnterState(const std::string &SubState) throw() = 0;

  /** called when a substate is left
   *
   *  @param SubState name of deactivated substate
   */
  virtual void handleQuitState(const std::string &SubState) throw() = 0;
};

}
}
