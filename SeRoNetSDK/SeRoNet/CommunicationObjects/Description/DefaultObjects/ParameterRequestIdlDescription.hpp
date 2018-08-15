//
// Created by ac120371 on 13.08.2018.
//

#pragma once

#include "../IDescription.hpp"
#include "../SelfDescriptionArray.hpp"
#include "../../DefaultObjects/CommParameterIDL/ParameterRequestIdl.hpp"

namespace SeRoNet {
namespace CommunicationObjects {
namespace Description {

template<>
SeRoNet::CommunicationObjects::Description::IDescription::shp_t SelfDescription<SeRoNet::CommunicationObjects::DefaultObjects::CommParameterIDL::NameValue>(
    SeRoNet::CommunicationObjects::DefaultObjects::CommParameterIDL::NameValue *obj,
    std::string name);

template<>
SeRoNet::CommunicationObjects::Description::IDescription::shp_t SelfDescription<DefaultObjects::CommParameterIDL::CommParameterRequest>(
    SeRoNet::CommunicationObjects::DefaultObjects::CommParameterIDL::CommParameterRequest *obj,
    std::string name);

}
}
}

