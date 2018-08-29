//
// Created by ac120371 on 13.08.2018.
//
#pragma once

#include "../../DefaultCommObjects/ParameterRequest.hpp"
#include "../../DefaultCommObjects/CommParameterIDL/ParameterRequestIdl.hpp"

#include "../../CommunicationObjects/Description/SelfDescription.hpp"


template<>
SeRoNet::CommunicationObjects::Description::IDescription::shp_t SeRoNet::CommunicationObjects::Description::SelfDescription<SeRoNet::DefaultCommObjects::CommParameterRequest>(
    SeRoNet::DefaultCommObjects::CommParameterRequest *obj,
    std::string name);
