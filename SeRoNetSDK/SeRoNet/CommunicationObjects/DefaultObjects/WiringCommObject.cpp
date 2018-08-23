//
// Created by ac120371 on 23.08.2018.
//

#include "WiringCommObject.hpp"

namespace SeRoNet {
namespace CommunicationObjects {
namespace DefaultObjects {

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

} // end namespace DefaultObjects
} // end namespace CommunicationObjects
} // end namespace SeRoNet