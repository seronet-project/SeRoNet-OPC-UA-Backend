//
// Created by ac120371 on 13.08.2018.
//

#ifndef SERONETSDK_PARAMETERRESPONSEDESCRIPTION_H
#define SERONETSDK_PARAMETERRESPONSEDESCRIPTION_H

#include "../../DefaultObjects/ParameterResponse.hpp"

#include "ParameterResponseIdlDescription.hpp"

namespace SeRoNet {
namespace CommunicationObjects {
namespace Description {
// serialization for CommBasePose
template<>
SeRoNet::CommunicationObjects::Description::IDescription::shp_t SelfDescription<DefaultObjects::CommParameterResponse>(
    DefaultObjects::CommParameterResponse *obj,
    std::string name);

}
}
}

#endif //SERONETSDK_PARAMETERRESPONSEDESCRIPTION_H
