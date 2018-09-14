//
// Created by ac120371 on 23.08.2018.
//

#pragma once

#include <string>
#include "../../DefaultCommObjects/WiringCommObject.hpp"
#include "../../CommunicationObjects/Description/SelfDescription.hpp"

namespace SeRoNet {
namespace CommunicationObjects {
namespace Description {

template<>
IDescription::shp_t SelfDescription(
    SeRoNet::DefaultCommObjects::WiringCommObject *obj,
    std::string name
);

}
}
}
