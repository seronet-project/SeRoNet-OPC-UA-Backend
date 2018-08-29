//
// Created by ac120371 on 23.08.2018.
//

#pragma once

#include <string>
#include "../../DefaultCommObjects/WiringCommObject.hpp"
#include "../../CommunicationObjects/Description/SelfDescription.hpp"


template<>
SeRoNet::CommunicationObjects::Description::IDescription::shp_t SeRoNet::CommunicationObjects::Description::SelfDescription(
    SeRoNet::DefaultCommObjects::WiringCommObject *obj,
    std::string name
);