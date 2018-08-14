//
// Created by ac120371 on 14.08.2018.
//
#include "ParameterResponseIdlDescription.hpp"
#include "../SelfDescriptionArray.hpp"
#include "../SelfDescriptionPrimitives.hpp"

template<>
SeRoNet::CommunicationObjects::Description::IDescription::shp_t SeRoNet::CommunicationObjects::Description::SelfDescription<
    SeRoNet::CommunicationObjects::DefaultObjects::CommParameterIDL::CommParameterResponse>(
    SeRoNet::CommunicationObjects::DefaultObjects::CommParameterIDL::CommParameterResponse *obj,
    std::string name) {

  SeRoNet::CommunicationObjects::Description::ComplexType::shp_t ret(
      new SeRoNet::CommunicationObjects::Description::ComplexType(name)
  );

  // Add Members
  (*ret).add(
      SelfDescription(&obj->response, "response")
  );
  return ret;
}