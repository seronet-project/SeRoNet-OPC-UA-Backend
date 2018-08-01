///
/// \file StateChangeHandler.hpp
/// \author Christian von Arnim
/// \date 30.07.2018
///

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
