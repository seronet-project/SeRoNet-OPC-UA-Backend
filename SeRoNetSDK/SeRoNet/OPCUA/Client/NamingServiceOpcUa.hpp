///
/// \file NamingServiceOpcUa.hpp
/// \author Christian von Arnim
/// \date 27.04.2018
///

#pragma once

#include <map>
#include <future>

#include "INamingService.hpp"

namespace SeRoNet {
namespace OPCUA {
namespace Client {

class NamingServiceOpcUa : public INamingService, public std::enable_shared_from_this<NamingServiceOpcUa> {
 public:
  ConnectionAndNodeid getConnectionAndNodeIdByName(const std::string &serverName, const std::string &service) override;
  typedef std::shared_ptr<NamingServiceOpcUa> shpType;
  typedef std::shared_ptr<UA_Client> shpUA_Client_t;
 protected:

  UaClientWithMutex_t::shpType getConnectionByName(const std::string &serverName);

  struct ConnectionAndThread
  {
    UaClientWithMutex_t::weakpType connection;
    std::thread *thread;
    /// \todo add possibility to kill the corresponding thread, save reference to thread, ...

  };

  /// \todo use one thread for all connections (loop for the connections in the thread
  void opcUaBackgroudTask(std::promise<shpUA_Client_t> promiseClient,
                          std::atomic_bool *run,
                          std::mutex &OPCUA_Mutex,
                          std::string serverURL);
  /// Custom Deleter of shared_ptr
  static void clientWithMutexDeletedCallback(UaClientWithMutex_t *pClientWithMutex, shpType pNamingService);

  std::map<std::string, ConnectionAndThread> m_connectionCache;
};

}
}
}