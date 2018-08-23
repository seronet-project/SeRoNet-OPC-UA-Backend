//
// Created by ac120371 on 23.08.2018.
//

#pragma once

#include <string>
#include "../../DefaultObjects/WiringCommObject.hpp"
#include "../SelfDescription.hpp"

namespace SeRoNet {
namespace CommunicationObjects {
namespace Description {

template<>
IDescription::shp_t SelfDescription(
    DefaultObjects::WiringCommObject *obj,
    std::string name
);

} // end namespace Description
} // end namespace CommunicationObjects
} // end namespace SeRoNet
