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

//@todo separate client management from namingService

class NamingServiceOpcUa : public INamingService, public std::enable_shared_from_this<NamingServiceOpcUa> {
 public:
  ConnectionAndNodeid getConnectionAndNodeIdByName(const std::string &serverName, const std::string &service) override;
  typedef std::shared_ptr<NamingServiceOpcUa> shpType;
  typedef std::shared_ptr<UA_Client> shpUA_Client_t;
 protected:

  UaClientWithMutex_t::shpType getConnectionByName(const std::string &serverName);

  std::string findServerUrlByServerName(const std::string &serverName);

  struct ConnectionAndThread {
    UaClientWithMutex_t::weakpType connection;
    std::thread *thread;
    std::shared_ptr<std::atomic_bool> pRun = std::make_shared<std::atomic_bool>(true);
  };

  void getNsIndexFromServer(const std::string &serverName);


  /// \todo use one thread for all connections (loop for the connections in the thread
  void opcUaBackgroundTask(std::promise<shpUA_Client_t> promiseClient,
                           std::atomic_bool *run,
                           std::mutex &OPCUA_Mutex,
                           std::string serverURL);
  /// Custom Deleter of shared_ptr
  static void clientWithMutexDeletedCallback(UaClientWithMutex_t *pClientWithMutex, shpType pNamingService, std::string serverName);

  static open62541::UA_NodeId createNodeId(const std::string &service, UA_UInt16 nsIndex);

  std::mutex m_connectionCacheMutex;
  std::map<std::string, ConnectionAndThread> m_connectionCache;
  UA_UInt16 m_nsIndex = UA_UINT16_MAX;
  void changeLocalHostname(std::string &discoveryUrl) const;
  std::string &getServerUrlFromLDS(const std::shared_ptr<UA_Client> &pClient,
                                   std::string &serverUrl,
                                   const std::string &discoveryUrl) const;
  std::string getServerNameFromServerOnNetworkElement(const std::string &searchServerName,
                                                      const UA_ServerOnNetwork *Element) const;
  std::string findServerUrlFromServerOnNetwork(const std::string &serverName,
                                               const UA_ServerOnNetwork *serverOnNetwork,
                                               size_t serverOnNetworkSize,
                                               const std::shared_ptr<UA_Client> &pClient
  ) const;
  std::string getServerUrlFromMdnsLDS(const std::string &serverName,
                                      const std::string &discoveryServerEndpoint,
                                      const std::shared_ptr<UA_Client> &pClient) const;
};

}
}
}