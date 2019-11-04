/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#ifndef SERONETSDK_PARAMETERRESPONSEIDL_H
#define SERONETSDK_PARAMETERRESPONSEIDL_H

#include "../EnumParamResponseType.hpp"

namespace SeRoNet {
namespace DefaultCommObjects {
namespace CommParameterIDL {


struct CommParameterResponse {
  SeRoNet::DefaultCommObjects::ENUM_ParamResponseType response;
};

} // end namespace CommParameterIDL
} // end namespace DefaultCommObjects
} // end namespace SeRoNet

#endif //SERONETSDK_PARAMETERRESPONSEIDL_H
