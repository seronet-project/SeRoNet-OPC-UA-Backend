/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#include "WiringCommObject.hpp"

namespace SeRoNet {
namespace DefaultCommObjects {

void WiringCommObject::setCommand(const std::string commandType,
                                  const std::string slaveport,
                                  const std::string servercomponent,
                                  const std::string serverservice) {
  this->commandType = commandType;
  this->slaveport = slaveport;
  this->servercomponent = servercomponent;
  this->serverservice = serverservice;

}

void WiringCommObject::getCommand(std::string &commandType,
                                  std::string &slaveport,
                                  std::string &servercomponent,
                                  std::string &serverservice) const {
  commandType = this->commandType;
  slaveport = this->slaveport;
  servercomponent = this->servercomponent;
  serverservice = this->serverservice;
}

} // end namespace DefaultCommObjects
} // end namespace CommunicationObjects
