///
/// \file ParameterResponseDescription.hpp
/// \author Christian von Arnim
/// \date 15.08.2018
///

#pragma once

#include <string>
#include "../../DefaultCommObjects/ParameterResponse.hpp"
#include "../../CommunicationObjects/Description/SelfDescription.hpp"

template<>
SeRoNet::CommunicationObjects::Description::IDescription::shp_t SeRoNet::CommunicationObjects::Description::SelfDescription(
    SeRoNet::DefaultCommObjects::CommParameterResponse *obj,
    std::string name
);
