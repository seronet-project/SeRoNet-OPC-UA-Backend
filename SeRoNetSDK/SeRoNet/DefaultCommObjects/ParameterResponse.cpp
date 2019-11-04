/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/


#include "ParameterResponse.hpp"

namespace SeRoNet {
namespace DefaultCommObjects {

CommunicationObjects::Description::ComplexType::shp_t CommParameterResponse::getObjectDescription(std::string name) {
  return
      SeRoNet::CommunicationObjects::Description::ComplexType::shp_t();
}

std::string CommParameterResponse::getName() const {
  return CommParameterResponse::identifier();
}

void CommParameterResponse::to_ostream(std::ostream &os) const {

  os << "CommParameterResponse(";
  os << getResponse();
  os << ")";

}

} // end namespace DefaultCommObjects
} // end namespace SeRoNet