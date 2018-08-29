///
/// \file ParameterResponseDescription.hpp
/// \author Christian von Arnim
/// \date 15.08.2018
///

#pragma once

#include <string>
#include "../../DefaultObjects/ParameterResponse.hpp"
#include "../SelfDescription.hpp"

namespace SeRoNet {
namespace CommunicationObjects {
namespace Description {

template<>
CommunicationObjects::Description::IDescription::shp_t SelfDescription(
    CommunicationObjects::DefaultObjects::CommParameterResponse *obj,
    std::string name
);

}
}
}
