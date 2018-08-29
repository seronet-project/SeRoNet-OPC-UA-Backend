///
/// \file ParameterResponseIdlDescription.hpp
/// \author Christian von Arnim
/// \date 15.08.2018
///

#pragma once
#include "../../CommunicationObjects/Description/SelfDescription.hpp"
#include "../../DefaultCommObjects/CommParameterIDL/ParameterResponseIdl.hpp"


template<>
SeRoNet::CommunicationObjects::Description::IDescription::shp_t SeRoNet::CommunicationObjects::Description::SelfDescription<
    SeRoNet::DefaultCommObjects::CommParameterIDL::CommParameterResponse>(
    SeRoNet::DefaultCommObjects::CommParameterIDL::CommParameterResponse *obj,
    std::string name);

