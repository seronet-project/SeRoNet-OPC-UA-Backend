/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/


#include <string>
#include "ParameterRequest.hpp"

namespace SeRoNet {
namespace DefaultCommObjects {

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

SeRoNet::CommunicationObjects::Description::ComplexType::shp_t CommParameterRequest::getObjectDescription(std::string name) {
  return SeRoNet::CommunicationObjects::Description::ComplexType::shp_t();
}

std::string CommParameterRequest::getName() const {
  return CommParameterRequest::identifier();
}

void CommParameterRequest::to_ostream(std::ostream &os) const {
  os << "CommParameterRequest(";

  std::vector<CommParameterIDL::NameValue>::const_iterator
      data_itemsIt;
  for (data_itemsIt
           = data.items.begin(); data_itemsIt
      != data.items.end(); data_itemsIt++) {

    os << "([" << (*data_itemsIt).key << "] ";
    for (unsigned i = 0; i < data_itemsIt->values.size(); ++i) {
      os << data_itemsIt->values[i] << " ";
    }
    os << ")";
  }
  os << ")";
}

} // end namespace DefaultCommObjects
} // end SeRoNet