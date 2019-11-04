/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#include "CommPose3dOpcUa.h"

#define SERONET_NO_DEPRECATED
#include <SeRoNet/CommunicationObjects/Description/ComplexType.hpp>
#include <SeRoNet/CommunicationObjects/Description/ElementPrimitives.hpp>
#include <SeRoNet/CommunicationObjects/Description/SelfDescriptionArray.hpp>
#include <SeRoNet/CommunicationObjects/Description/ElementArray.hpp>

namespace SeRoNet {
namespace CommunicationObjects {
namespace Description {

// serialization for CommBasicObjectsIDL::CommPosition3d
template <>
IDescription::shp_t SelfDescription(CommPose3d *obj, std::string name)
{
	auto ret = std::make_shared<SeRoNet::CommunicationObjects::Description::ComplexType>(name);
	// add x
	ret->add(
		SelfDescription(&(obj->x), "X")
	);
	// add y
	ret->add(
		SelfDescription(&(obj->y), "Y")
	);
	// add z
	ret->add(
		SelfDescription(&(obj->z), "Z")
	);
	return ret;
} // end SelfDescription

} // end namespace Description
} // end namespace CommunicationObjects
} // end namespace SeRoNet

