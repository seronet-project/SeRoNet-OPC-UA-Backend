//
// Created by ac120371 on 13.08.2018.
//
#pragma once

#include "../../DefaultObjects/ParameterRequest.hpp"
#include "../../DefaultObjects/CommParameterIDL/ParameterRequestIdl.hpp"

#include "../SelfDescription.hpp"

namespace SeRoNet {
namespace CommunicationObjects {
namespace Description {

template<>
SeRoNet::CommunicationObjects::Description::IDescription::shp_t SelfDescription<SeRoNet::CommunicationObjects::DefaultObjects::CommParameterRequest>(
    DefaultObjects::CommParameterRequest *obj,
    std::string name);

}
}
}
