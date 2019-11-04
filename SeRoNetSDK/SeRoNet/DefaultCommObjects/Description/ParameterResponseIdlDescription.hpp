/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#pragma once

#include "../../../Definitions.hpp"
#include "../../CommunicationObjects/Description/SelfDescription.hpp"
#include "../../DefaultCommObjects/CommParameterIDL/ParameterResponseIdl.hpp"

namespace SeRoNet {
namespace CommunicationObjects {
namespace Description {

template<>
SERONETSDK_EXPORT IDescription::shp_t SelfDescription<
    SeRoNet::DefaultCommObjects::CommParameterIDL::CommParameterResponse>(
    SeRoNet::DefaultCommObjects::CommParameterIDL::CommParameterResponse *obj,
    std::string name);

}
}
}
