//
// Created by ac120371 on 14.08.2018.
//
#include "ParameterResponseDescription.hpp"
#include "ParameterResponseIdlDescription.hpp"
#include "../../CommunicationObjects/Description/ComplexType.hpp"



template<>
SeRoNet::CommunicationObjects::Description::IDescription::shp_t SeRoNet::CommunicationObjects::Description::SelfDescription(
        SeRoNet::DefaultCommObjects::CommParameterResponse *obj,
        std::string name
    ) {

  SeRoNet::CommunicationObjects::Description::ComplexType::shp_t ret(
      new SeRoNet::CommunicationObjects::Description::ComplexType(name)
  );

//  Add Members
  (*ret).add(
      SelfDescription(&obj->data, "data")
  );

  return ret;
}

