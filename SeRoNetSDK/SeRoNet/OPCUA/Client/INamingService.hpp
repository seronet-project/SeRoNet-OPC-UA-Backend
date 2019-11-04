/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#pragma once
#include <string>

#include "UaClientWithMutex.hpp"
#include <Open62541Cpp/UA_NodeId.hpp>

namespace SeRoNet {
namespace OPCUA {
namespace Client {

class INamingService {
 public:

  struct ConnectionAndNodeid{
    UaClientWithMutex_t::shpType connection;
    OPEN_65241_CPP_NAMESPACE::UA_NodeId nodeId;
  };

  virtual ConnectionAndNodeid getConnectionAndNodeIdByName(const std::string &serverName, const std::string &service) = 0;
};

}
}
}
