/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#pragma once

#include <open62541/client.h>
#include <memory>
#include <mutex>

namespace SeRoNet {
namespace OPCUA {
namespace Client {
struct UaClientWithMutex_t {
  std::shared_ptr<UA_Client> pClient;
  std::mutex opcuaMutex;
  typedef std::shared_ptr<UaClientWithMutex_t> shpType;
  typedef std::weak_ptr<UaClientWithMutex_t> weakpType;
};
}
}
}


