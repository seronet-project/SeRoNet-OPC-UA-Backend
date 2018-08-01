///
/// \file CommObjs.hpp
/// \author Christian von Arnim
/// \date 30.07.2018
///

#pragma once

#include <string>
#include <vector>

#include "../../../SmartSoftComponentDeveloperAPIcpp/SmartSoft_CD_API/smartICommunicationObject.h"

namespace SeRoNet {

namespace State {

enum StateCommand {
  STATE_CMD_ERROR,
  STATE_CMD_SET_STATE,
  STATE_CMD_GET_CURRENT_STATE,
  STATE_CMD_GET_MAIN_STATES,
  STATE_CMD_GET_SUB_STATES
};

class CommStateRequest : public Smart::ICommunicationObject{
 public:
  // internal data structure
  int command;
  std::string getName() const override {
    return __FUNCTION__;
  }

  std::string state_name;
  //
  // constructors, destructors, copy constructors etc. ...
  //
  CommStateRequest() {
    command = static_cast<int>(STATE_CMD_ERROR);
  }

  virtual ~CommStateRequest() {};

  static inline std::string identifier(void) {
    return "SmartACE::smartStateRequest";
  };

  /// Set/Get the command-enum defined above
  inline void setCommand(const StateCommand &command) {
    this->command = static_cast<int>(command);
  }
  inline StateCommand getCommand(void) const {
    return static_cast<StateCommand>(this->command);
  }

  /// Set/Get state-name
  inline void setStateName(const std::string &name) {
    state_name = name;
  }
  inline std::string getStateName(void) const {
    return state_name;
  }
};

/////////////////////////////////////////////////////////////////////////
//
// Response (Answer)
//
/////////////////////////////////////////////////////////////////////////

class CommStateResponse : public Smart::ICommunicationObject{
 public:

  std::string getName() const override {
    return __FUNCTION__;
  }

  // internal data structure
  int status;
  std::vector<std::string> state_list;

  //
  // constructors, destructors, copy constructors etc. ...
  //
  CommStateResponse() {
    status = 0;
    state_list.clear();
  }
  virtual ~CommStateResponse() {}

  static inline std::string identifier(void) {
    return "smartStateResponse";
  };

  /// Get/Set return status
  inline void setStatus(const int &_status) {
    status = _status;
  }
  inline int getStatus(void) const {
    return status;
  }

  ///
  inline void setStateList(const std::vector<std::string> &states) {
    state_list = states;
  }
  inline std::vector<std::string> getStateList(void) const {
    return state_list;
  }
};

}
}
