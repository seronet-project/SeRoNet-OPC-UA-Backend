///
/// \file ParameterRequestIdlDescription.hpp
/// \author Christian von Arnim
/// \date 15.08.2018
///

#pragma once

#include "../../CommunicationObjects/Description/SelfDescription.hpp"
#include "../../DefaultCommObjects/ParameterRequest.hpp"

namespace SeRoNet {
namespace CommunicationObjects {
namespace Description {

template<>
IDescription::shp_t SelfDescription
<SeRoNet::DefaultCommObjects::CommParameterIDL::NameValue>
(
    SeRoNet::DefaultCommObjects::CommParameterIDL::NameValue *obj,
    std::string name);

template<>
IDescription::shp_t SelfDescription<
    SeRoNet::DefaultCommObjects::CommParameterIDL::CommParameterRequest>(
    SeRoNet::DefaultCommObjects::CommParameterIDL::CommParameterRequest *obj,
    std::string name);

}
}
}
