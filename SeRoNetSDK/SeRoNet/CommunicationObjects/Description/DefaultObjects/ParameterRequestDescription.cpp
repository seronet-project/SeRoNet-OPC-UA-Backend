//
// Created by ac120371 on 14.08.2018.
//

#include "ParameterRequestDescription.hpp"

template<>
SeRoNet::CommunicationObjects::Description::IDescription::shp_t SeRoNet::CommunicationObjects::Description::SelfDescription<
    SeRoNet::CommunicationObjects::DefaultObjects::CommParameterRequest>(
    SeRoNet::CommunicationObjects::DefaultObjects::CommParameterRequest *obj,
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
