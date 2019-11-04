/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#ifndef SERONETSDK_WIRING_HPP
#define SERONETSDK_WIRING_HPP

#include <string>
#include <smartStatusCode.h>

namespace SeRoNet {
namespace DefaultCommObjects {

class WiringCommObject {

 protected:
  std::string commandType = "";
  std::string slaveport = "";
  std::string servercomponent = "";
  std::string serverservice = "";
  std::int32_t status = 0;

 public:
  std::string getCommandType() { return this->commandType; }

  void setCommandType(std::string commandType) {
    this->commandType = commandType;
  };

  std::string getSlaveport() const {
    return this->slaveport;
  }
  void setSlaveport(std::string slaveport) {
    this->slaveport = slaveport;
  }

  std::string getServercomponent() {
    return this->servercomponent;
  }
  void setServercomponent(std::string servercomponent) {
    this->servercomponent = servercomponent;
  }

  std::string getServerservice() {
    return this->serverservice;
  }
  void setServerservice(std::string serverservice) {
    this->serverservice = serverservice;
  }

  std::int32_t getStatus() {
    return this->status;
  }

  void setStatus(std::int32_t status) {
    this->status = status;
  }

  //
  // constructors, destructors, copy constructors etc. ...
  //
  WiringCommObject() = default;
  virtual ~WiringCommObject() = default;

  static inline std::string identifier() {
    return "SmartACE::smartWiring";
  };

  void setCommand(const std::string command,
                  const std::string slaveport,
                  const std::string servercomponent,
                  const std::string serverservice);

  void getCommand(std::string &command,
                  std::string &slaveport,
                  std::string &servercomponent,
                  std::string &serverservice) const;

  void setStatus(const Smart::StatusCode status) { this->status = status; };
  void getStatus(Smart::StatusCode &status) { status = (Smart::StatusCode) this->getStatus(); };

};

} // end namespace DefaultCommObjects
} // end namespace SeRoNet


#endif //SERONETSDK_WIRING_HPP
