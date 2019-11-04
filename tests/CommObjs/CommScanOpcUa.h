/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#ifndef COMMSCANOPCUA_H_
#define COMMSCANOPCUA_H_

#include "CommScan.h"

#pragma once
#include <SeRoNet/CommunicationObjects/Description/SelfDescription.hpp>
#include "CommScan.h"

namespace SeRoNet {
namespace CommunicationObjects {
namespace Description {

// serialization for CommBasicObjectsIDL::CommPose3d
template <>
IDescription::shp_t SelfDescription(CommScan *obj, std::string name);

} // end namespace Description
} // end namespace CommunicationObjects
} // end namespace SeRoNet
#endif /* COMMSCANOPCUA_H_ */
