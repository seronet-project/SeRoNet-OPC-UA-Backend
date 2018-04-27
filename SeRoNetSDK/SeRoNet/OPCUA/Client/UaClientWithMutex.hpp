///
/// @file UaClientWithMutex.hpp
/// @author Christian von Arnim 
/// @date 10.01.2018
///

#pragma once

#include <open62541.h>
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


