//
// Created by ac120371 on 14.08.2018.
//
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

