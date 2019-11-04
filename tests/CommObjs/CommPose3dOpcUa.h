/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#ifndef COMMPOSE3DOPCUA_H_
#define COMMPOSE3DOPCUA_H_

#include "CommPose3d.h"

#include <SeRoNet/CommunicationObjects/Description/SelfDescription.hpp>
#pragma once

namespace SeRoNet {
namespace CommunicationObjects {
namespace Description {

// serialization for CommBasicObjectsIDL::CommPose3d
template <>
IDescription::shp_t SelfDescription(CommPose3d *obj, std::string name);

} // end namespace Description
} // end namespace CommunicationObjects
} // end namespace SeRoNet
#endif /* COMMPOSE3DOPCUA_H_ */
