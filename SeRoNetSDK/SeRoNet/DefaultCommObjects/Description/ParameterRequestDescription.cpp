//
// Created by ac120371 on 14.08.2018.
//

#include "ParameterRequestDescription.hpp"
#include "ParameterRequestIdlDescription.hpp"

template<>
SeRoNet::CommunicationObjects::Description::IDescription::shp_t SeRoNet::CommunicationObjects::Description::SelfDescription<
    SeRoNet::DefaultCommObjects::CommParameterRequest>(
    SeRoNet::DefaultCommObjects::CommParameterRequest *obj,
    std::string name) {

  SeRoNet::CommunicationObjects::Description::ComplexType::shp_t ret(
      new SeRoNet::CommunicationObjects::Description::ComplexType(name)
  );

  // Add Member

  (*ret).add(
      SelfDescription(&obj->data, "data")
  );

  return ret;
}
