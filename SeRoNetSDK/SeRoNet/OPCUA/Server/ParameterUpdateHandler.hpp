/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/


#ifndef SERONETSDK_PARAMETERUPDATEHANDLER_HPP
#define SERONETSDK_PARAMETERUPDATEHANDLER_HPP

#include "../../DefaultCommObjects/ParameterResponse.hpp"
#include "../../DefaultCommObjects/ParameterRequest.hpp"

class ParameterSlave;

namespace SeRoNet {
namespace OPCUA {
namespace Server {

class ParameterUpdateHandler {
 public:
  ParameterUpdateHandler() = default;
  virtual ~ParameterUpdateHandler() = default;
  virtual SeRoNet::DefaultCommObjects::CommParameterResponse handleParameter(const SeRoNet::DefaultCommObjects::CommParameterRequest &request)=0;
};

}
}
}

#endif //SERONETSDK_PARAMETERUPDATEHANDLER_HPP
