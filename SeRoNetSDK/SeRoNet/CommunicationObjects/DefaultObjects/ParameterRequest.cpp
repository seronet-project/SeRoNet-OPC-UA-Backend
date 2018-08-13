//
// Created by ac120371 on 13.08.2018.
//

#include <string>
#include "ParameterRequest.h"

namespace SeRoNet {
namespace CommunicationObjects {
namespace DefaultObjects {

int CommParameterRequest::getString(const std::string &key, std::string &value) const {
  return get(key, value);
}

std::string CommParameterRequest::getString(const std::string &key) const {
  std::string result;
  this->get(key, result);
  return result;
}

int CommParameterRequest::getStringList(const std::string &key, std::list<std::string> &value) const {
  return getList(key, value);
}

std::list<std::string> CommParameterRequest::getStringList(const std::string &key) const {
  std::list<std::string> result;
  this->getList(key, result);
  return result;
}

// getTag returns the string value for the top level tag
std::string CommParameterRequest::getTag() const {
  return getString("slot");
}

// setTag writes the string value for the top level tag
void CommParameterRequest::setTag(const std::string &value) {
  setString("slot", value);
}

Description::ComplexType::shp_t CommParameterRequest::getObjectDescription(std::string name) {
  return SeRoNet::CommunicationObjects::Description::ComplexType::shp_t();
}

std::string CommParameterRequest::getName() const {
  return CommParameterRequest::identifier();
}

} // end namespace DefaultObjects
} // end namespace CommunicationObjects
} // end SeRoNet