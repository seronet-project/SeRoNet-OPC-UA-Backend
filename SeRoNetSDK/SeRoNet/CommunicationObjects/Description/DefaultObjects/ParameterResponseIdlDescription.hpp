//
// Created by ac120371 on 13.08.2018.
//

#ifndef SERONETSDK_PARAMETERRESPONSEIDLDESCRIPTION_H
#define SERONETSDK_PARAMETERRESPONSEIDLDESCRIPTION_H

#include "../SelfDescriptionArray.hpp"
#include "../SelfDescriptionPrimitives.hpp"
#include "../IDescription.hpp"
#include "../../DefaultObjects/CommParameterIDL/ParameterResponseIdl.hpp"

namespace SeRoNet {
namespace CommunicationObjects {
namespace Description {

template<>
SeRoNet::CommunicationObjects::Description::IDescription::shp_t SelfDescription<SeRoNet::CommunicationObjects::DefaultObjects::CommParameterIDL::CommParameterResponse>(
    SeRoNet::CommunicationObjects::DefaultObjects::CommParameterIDL::CommParameterResponse *obj,
    std::string name);

} // end namespace DefaultObjects
} // end namespace Description
} // end namespace SeRoNet

#endif //SERONETSDK_PARAMETERRESPONSEIDLDESCRIPTION_H
