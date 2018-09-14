///
/// \file ParameterResponseDescription.hpp
/// \author Christian von Arnim
/// \date 15.08.2018
///

#pragma once

#include <string>
#include "../../DefaultCommObjects/ParameterResponse.hpp"
#include "../../CommunicationObjects/Description/SelfDescription.hpp"

namespace SeRoNet {
namespace CommunicationObjects {
namespace Description {

template<>
IDescription::shp_t SelfDescription(
    SeRoNet::DefaultCommObjects::CommParameterResponse *obj,
    std::string name
);

}
}
}
