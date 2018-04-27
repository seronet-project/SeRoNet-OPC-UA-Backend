///
/// \file INamingService.hpp
/// \author Christian von Arnim
/// \date 27.04.2018
///

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
