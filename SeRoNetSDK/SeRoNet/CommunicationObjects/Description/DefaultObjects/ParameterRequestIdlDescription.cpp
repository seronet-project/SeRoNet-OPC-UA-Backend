//
// Created by ac120371 on 14.08.2018.
//
#include "ParameterRequestIdlDescription.hpp"
#include "../ComplexType.hpp"
#include "../ElementArray.hpp"
#include "../ElementPrimitives.hpp"

template<>
SeRoNet::CommunicationObjects::Description::IDescription::shp_t SeRoNet::CommunicationObjects::Description::SelfDescription<
    SeRoNet::CommunicationObjects::DefaultObjects::CommParameterIDL::NameValue>(
    SeRoNet::CommunicationObjects::DefaultObjects::CommParameterIDL::NameValue *obj,
    std::string name) {

  SeRoNet::CommunicationObjects::Description::ComplexType::shp_t ret(
      new SeRoNet::CommunicationObjects::Description::ComplexType(name)
  );

  // Add Members
  (*ret).add(
      SeRoNet::CommunicationObjects::Description::IDescription::shp_t(
          new SeRoNet::CommunicationObjects::Description::ElementPrimitive<std::string>(
              "key",
              std::bind(&SeRoNet::CommunicationObjects::DefaultObjects::CommParameterIDL::NameValue::getKey, obj),
              std::bind(&SeRoNet::CommunicationObjects::DefaultObjects::CommParameterIDL::NameValue::setKey,
                        obj,
                        std::placeholders::_1)
          )
      )
  ).add(
      SeRoNet::CommunicationObjects::Description::IDescription::shp_t(
          new SeRoNet::CommunicationObjects::Description::ArrayOf<std::string>(
              "value",
              std::bind(&SeRoNet::CommunicationObjects::DefaultObjects::CommParameterIDL::NameValue::getValues, obj),
              std::bind(&SeRoNet::CommunicationObjects::DefaultObjects::CommParameterIDL::NameValue::setValues,
                        obj,
                        std::placeholders::_1)
          )
      )
  );

  return ret;
}

template<>
SeRoNet::CommunicationObjects::Description::IDescription::shp_t SeRoNet::CommunicationObjects::Description::SelfDescription<
    SeRoNet::CommunicationObjects::DefaultObjects::CommParameterIDL::CommParameterRequest>(
    SeRoNet::CommunicationObjects::DefaultObjects::CommParameterIDL::CommParameterRequest *obj,
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