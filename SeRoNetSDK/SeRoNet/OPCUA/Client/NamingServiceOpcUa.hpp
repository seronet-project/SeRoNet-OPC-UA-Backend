/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#pragma once

#include "../../../Definitions.hpp"
#include <map>
#include <future>
#include "../Server/OpcuaServer.hpp"

#include "INamingService.hpp"

namespace SeRoNet {
namespace OPCUA {
namespace Client {

//@todo separate client management from namingService

class SERONETSDK_EXPORT NamingServiceOpcUa : public INamingService, public std::enable_shared_from_this<NamingServiceOpcUa> {
 public:
  NamingServiceOpcUa(std::shared_ptr<SeRoNet::OPCUA::Server::OpcUaServer> pOpcUaServer);

  ConnectionAndNodeid getConnectionAndNodeIdByName(const std::string &serverName, const std::string &service) override;
  typedef std::shared_ptr<NamingServiceOpcUa> shpType;
  typedef std::shared_ptr<UA_Client> shpUA_Client_t;

 protected:

  struct ConnectionAndThread {
    UaClientWithMutex_t::weakpType connection;
    std::thread thread;
    std::shared_ptr<std::atomic_bool> pRun = std::make_shared<std::atomic_bool>(true);
  };

  UaClientWithMutex_t::shpType getConnectionByName(const std::string &serverName);

  UA_UInt16 getNsIndexFromServer(const std::string &serverName, UaClientWithMutex_t::shpType aConnection);


  std::string findServerUrlByServerName(const std::string &serverName);


  /// \todo use one thread for all connections (loop for the connections in the thread
  void opcUaBackgroundTask(std::promise<shpUA_Client_t> promiseClient,
                           std::atomic_bool *run,
                           std::mutex &OPCUA_Mutex,
                           std::string serverURL);
  /// Custom Deleter of shared_ptr
  static void clientWithMutexDeletedCallback(UaClientWithMutex_t *pClientWithMutex, shpType pNamingService, std::string serverName);

  static open62541Cpp::UA_NodeId createNodeId(const std::string &service, UA_UInt16 nsIndex);

  void changeLocalHostname(std::string &discoveryUrl) const;
  std::string getSessionEndpointUrlFromDiscoveryEndpointUrl(const std::shared_ptr<UA_Client> &pClient,
                                                            const std::string &discoveryUrl) const;
  std::string getServerNameFromServerOnNetworkElement(const UA_ServerOnNetwork *Element) const;
  std::string findServerUrlFromServerOnNetwork(const std::string &serverName,
                                               std::multimap<std::string, std::string>,
                                               const std::shared_ptr<UA_Client> &pClient
  ) const;
  std::string getServerUrlFromMdnsLDS(const std::string &serverName,
                                      const std::string &discoveryServerEndpoint,
                                      const std::shared_ptr<UA_Client> &pClient) const;

  std::map<std::string, std::string> m_serverUrl;
  std::shared_ptr<SeRoNet::OPCUA::Server::OpcUaServer> m_pOpcUaServer;
  std::mutex m_connectionCacheMutex;
  std::map<std::string, std::shared_ptr<ConnectionAndThread>> m_connectionCache;
};

}
}
}