/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#include "ParameterRequestDescription.hpp"
#include "ParameterRequestIdlDescription.hpp"

namespace SeRoNet {
namespace CommunicationObjects {
namespace Description {

template<>
IDescription::shp_t SelfDescription<
    SeRoNet::DefaultCommObjects::CommParameterRequest>(
    SeRoNet::DefaultCommObjects::CommParameterRequest *obj,
    std::string name) {

  ComplexType::shp_t ret(
      new ComplexType(name)
  );

  // Add Member

  (*ret).add(
      SelfDescription(&obj->data, "data")
  );

  return ret;
}

}
}
}
