/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/


#include "../../CommunicationObjects/Description/ComplexType.hpp"
#include "../../CommunicationObjects/Description/ElementArray.hpp"
#include "../../CommunicationObjects/Description/ElementPrimitives.hpp"
#include "../../CommunicationObjects/Description/SelfDescriptionArray.hpp"
#include "ParameterRequestIdlDescription.hpp"

template<>
SeRoNet::CommunicationObjects::Description::IDescription::shp_t SeRoNet::CommunicationObjects::Description::SelfDescription(
    SeRoNet::DefaultCommObjects::CommParameterIDL::NameValue *obj,
    std::string name) {

  SeRoNet::CommunicationObjects::Description::ComplexType::shp_t ret(
      new SeRoNet::CommunicationObjects::Description::ComplexType(name)
  );

  // Add Members
  (*ret).add(
      SeRoNet::CommunicationObjects::Description::IDescription::shp_t(
          new SeRoNet::CommunicationObjects::Description::ElementPrimitive<std::string>(
              "key",
              std::bind(&SeRoNet::DefaultCommObjects::CommParameterIDL::NameValue::getKey, obj),
              std::bind(&SeRoNet::DefaultCommObjects::CommParameterIDL::NameValue::setKey,
                        obj,
                        std::placeholders::_1)
          )
      )
  ).add(
      SeRoNet::CommunicationObjects::Description::IDescription::shp_t(
          new SeRoNet::CommunicationObjects::Description::ArrayOf<std::string>(
              "value",
              std::bind(&SeRoNet::DefaultCommObjects::CommParameterIDL::NameValue::getValues, obj),
              std::bind(&SeRoNet::DefaultCommObjects::CommParameterIDL::NameValue::setValues,
                        obj,
                        std::placeholders::_1)
          )
      )
  );

  return ret;
}


template<>
SeRoNet::CommunicationObjects::Description::IDescription::shp_t SeRoNet::CommunicationObjects::Description::SelfDescription<
    SeRoNet::DefaultCommObjects::CommParameterIDL::CommParameterRequest>(
    SeRoNet::DefaultCommObjects::CommParameterIDL::CommParameterRequest *obj,
    std::string name) {

  SeRoNet::CommunicationObjects::Description::ComplexType::shp_t ret(
      new SeRoNet::CommunicationObjects::Description::ComplexType(name)
  );

  // Add Members
  (*ret).add(
      SelfDescription(&obj->items, "items")
  );
  return ret;
}

