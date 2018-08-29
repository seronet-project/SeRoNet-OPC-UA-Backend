///
/// \file ParameterRequestIdlDescription.hpp
/// \author Christian von Arnim
/// \date 15.08.2018
///

#pragma once

#include "../../CommunicationObjects/Description/SelfDescription.hpp"
#include "../../DefaultCommObjects/ParameterRequest.hpp"


template<>
SeRoNet::CommunicationObjects::Description::IDescription::shp_t SeRoNet::CommunicationObjects::Description::SelfDescription
<SeRoNet::DefaultCommObjects::CommParameterIDL::NameValue>
(
    SeRoNet::DefaultCommObjects::CommParameterIDL::NameValue *obj,
    std::string name);

template<>
SeRoNet::CommunicationObjects::Description::IDescription::shp_t SeRoNet::CommunicationObjects::Description::SelfDescription<
    SeRoNet::DefaultCommObjects::CommParameterIDL::CommParameterRequest>(
    SeRoNet::DefaultCommObjects::CommParameterIDL::CommParameterRequest *obj,
    std::string name);
