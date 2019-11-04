/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#include "ParameterResponseIdlDescription.hpp"
#include "../../CommunicationObjects/Description/SelfDescriptionArray.hpp"
#include "../../CommunicationObjects/Description/SelfDescriptionPrimitives.hpp"
#include "../../CommunicationObjects/Description/ComplexType.hpp"

template<>
SeRoNet::CommunicationObjects::Description::IDescription::shp_t SeRoNet::CommunicationObjects::Description::SelfDescription(
    SeRoNet::DefaultCommObjects::CommParameterIDL::CommParameterResponse *obj,
    std::string name)
    {

      SeRoNet::CommunicationObjects::Description::ComplexType::shp_t ret(
      new ComplexType(name)
  );

  // Add Members
  (*ret).add(
      SelfDescription(&obj->response, "response")
  );
  return ret;
}

