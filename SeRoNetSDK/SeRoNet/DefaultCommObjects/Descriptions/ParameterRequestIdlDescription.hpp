///
/// \file ParameterRequestIdlDescription.hpp
/// \author Christian von Arnim
/// \date 15.08.2018
///

#pragma once

#include "../SelfDescription.hpp"
#include "../../DefaultObjects/ParameterRequest.hpp"

namespace SeRoNet {
namespace CommunicationObjects {
namespace Description {

template<>
IDescription::shp_t SelfDescription
<SeRoNet::CommunicationObjects::DefaultObjects::CommParameterIDL::NameValue>
(
    SeRoNet::CommunicationObjects::DefaultObjects::CommParameterIDL::NameValue *obj,
    std::string name);

template<>
IDescription::shp_t SelfDescription<
    CommunicationObjects::DefaultObjects::CommParameterIDL::CommParameterRequest>(
    CommunicationObjects::DefaultObjects::CommParameterIDL::CommParameterRequest *obj,
    std::string name);

}
}
}
