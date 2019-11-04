/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/


#include <iostream>
#include <functional>
#include <map>
#include <open62541/client_highlevel.h>
#include <open62541/client_config_default.h>
#include <Open62541Cpp/UA_String.hpp>

#include <Open62541Cpp/UA_Variant.hpp>
#include <Open62541Cpp/UA_ArrayOfVariant.hpp>

#include <sstream>
#include <Open62541Cpp/Exceptions/OpcUaErrorException.hpp>

#include "NamingServiceOpcUa.hpp"
#include "../../Exceptions/SeRoNetSDKException.hpp"
#include "../../Exceptions/InvalidConversion.hpp"
#include "../Server/OpcuaServer.hpp"

#define DISCOVERY_SERVER_ENDPOINT "opc.tcp://localhost"

namespace SeRoNet {
namespace OPCUA {
namespace Client {

INamingService::ConnectionAndNodeid NamingServiceOpcUa::getConnectionAndNodeIdByName(const std::string &serverName,
                                                                                     const std::string &service) {

  ConnectionAndNodeid ret;
  ret.connection = getConnectionByName(serverName);
  auto nsIndex = this->getNsIndexFromServer(serverName, ret.connection);
  ret.nodeId = createNodeId(service, nsIndex);

  return ret;
}

UA_UInt16 NamingServiceOpcUa::getNsIndexFromServer(const std::string &serverName,
                                                   UaClientWithMutex_t::shpType aConnection) {
  const UA_NodeId nodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_SERVER_NAMESPACEARRAY);
  const open62541::UA_String searchNamespace{"http://seronet-projekt.de/"};
  UA_StatusCode retval;
  open62541::UA_ArrayOfVariant nsArray(1);

  std::unique_lock<decltype(aConnection.get()->opcuaMutex)> lock(aConnection.get()->opcuaMutex);
  retval = UA_Client_readValueAttribute(aConnection.get()->pClient.get(), nodeId, nsArray.Variants);
  if (retval != UA_STATUSCODE_GOOD) throw open62541::Exceptions::OpcUaErrorException(retval);

  open62541::UA_Variant returnValue = nsArray[0];
  if (!UA_Variant_hasArrayType(returnValue.Variant,
                               &UA_TYPES[UA_TYPES_STRING]))
    throw SeRoNet::Exceptions::InvalidConversion("NamespaceArray is not a Array");

  for (int i = 0; i < returnValue.Variant->arrayLength; i++) {
    auto element = (UA_String *) returnValue.Variant->data;
    open62541::UA_String currentNamespace = open62541::UA_String(&element[i]);
    if (currentNamespace == searchNamespace) {
      std::cout << "Namespace Index of " << currentNamespace << " : " << (UA_UInt16) i << std::endl;
      return (UA_UInt16) i;
    }
  }
  throw SeRoNet::Exceptions::InvalidConversion("No Namespaceindex found");
}

UaClientWithMutex_t::shpType NamingServiceOpcUa::getConnectionByName(const std::string &serverName) {

  {
    std::unique_lock<decltype(m_connectionCacheMutex)> ul_connCache;
    // Check if cache availiable
    auto it = m_connectionCache.find(serverName);
    if (it != m_connectionCache.end()) {
      // Check if pointer is valid
      auto connAndThread = it->second;
      auto shpConn = connAndThread->connection.lock();
      if (!shpConn) {
        // Remove from cache
        m_connectionCache.erase(it);
      } else {
        // Connection still valid, return
        return shpConn;
      }
    }
  }

  // No connection in the cache, create a new one
  {

    std::string serverUrl = findServerUrlByServerName(serverName);

    auto connAndThread = std::make_shared<ConnectionAndThread>();

    SeRoNet::OPCUA::Client::UaClientWithMutex_t::shpType
        shpUaClientWithMutex(
        new SeRoNet::OPCUA::Client::UaClientWithMutex_t,
        std::bind(clientWithMutexDeletedCallback, std::placeholders::_1, shared_from_this(), serverName)
    );
    connAndThread->connection = shpUaClientWithMutex;

    // std::mutex OPCUA_Mutex;
    std::promise<shpUA_Client_t> promiseClient;
    std::future<shpUA_Client_t> futureClient = promiseClient.get_future();

    std::thread opcUA_Thread(
            &NamingServiceOpcUa::opcUaBackgroundTask,
            this,
            std::move(promiseClient),
            connAndThread->pRun.get(),
            std::ref(shpUaClientWithMutex->opcuaMutex),
            serverUrl
        );

    // Wait until client is set
    shpUA_Client_t pClient = futureClient.get();
    if (pClient == nullptr) {
      std::cerr << "No valid client" << std::endl;
      return UaClientWithMutex_t::shpType();
    }
    shpUaClientWithMutex->pClient = pClient;

    /// \todo add shpUaClientWithMutex to cache

    {
      std::unique_lock<decltype(m_connectionCacheMutex)> ul_connCache;
      connAndThread->thread = std::move(opcUA_Thread);
      connAndThread->connection = shpUaClientWithMutex;

      m_connectionCache.insert(std::make_pair(serverName, connAndThread));
    }

    return shpUaClientWithMutex;

  }
}

void deleteUaClient(UA_Client *pClient) {
  if (pClient != nullptr) {
    UA_Client_disconnect(pClient);
    UA_Client_delete(pClient);
  }
}

void NamingServiceOpcUa::clientWithMutexDeletedCallback(
    UaClientWithMutex_t *pClientWithMutex,
    NamingServiceOpcUa::shpType pNamingService,
    std::string serverName
) {
  /// \todo Kill the worker thread of the UaClient
  std::unique_lock<decltype(pNamingService->m_connectionCacheMutex)> ul_connCache;
  auto it = pNamingService->m_connectionCache.find(serverName);
  if (it != pNamingService->m_connectionCache.end()) {
    *(it->second->pRun) = false;
    if (it->second->thread.joinable()) {
      it->second->thread.join();
    }
  }
  pNamingService->m_connectionCache.erase(it);
  delete pClientWithMutex;
}

void NamingServiceOpcUa::opcUaBackgroundTask(std::promise<shpUA_Client_t> promiseClient,
                                             std::atomic_bool *run,
                                             std::mutex &OPCUA_Mutex,
                                             std::string serverURL) {
  // Initialize shared pointer so it will clean up UA_Client*
  shpUA_Client_t pClient(UA_Client_new(), deleteUaClient);
  UA_ClientConfig_setDefault(UA_Client_getConfig(pClient.get()));

  UA_StatusCode retVal;

  for(int i = 9; i >= 0; --i)
  {
    /* Connect to a server */
    std::cout << "Connecting to: " << serverURL << std::endl;
    retVal = UA_Client_connect(pClient.get(), serverURL.c_str());
    if (retVal == UA_STATUSCODE_GOOD) {
      break;
    }

    std::cout << "Connection failed: " << UA_StatusCode_name(retVal) << " Remaining retries: " << i << std::endl;
  }

  if (retVal != UA_STATUSCODE_GOOD) {
    pClient = nullptr;
    promiseClient.set_value(pClient);
    return;
  }

  promiseClient.set_value(pClient);
  /// OPC UA Client Loop, acquire lock while checking for new messages
  {
    /// Initialize without locking
    std::unique_lock<std::remove_reference<decltype(OPCUA_Mutex)>::type> l_opcUA(OPCUA_Mutex, std::defer_lock);
    while (*run) {
      l_opcUA.lock();
      UA_Client_run_iterate(pClient.get(), 10);
      l_opcUA.unlock();
      std::this_thread::yield();
      std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
  }
}


std::string NamingServiceOpcUa::findServerUrlByServerName(const std::string &serverName) {

  std::stringstream ss;
  std::thread *backgroundLDSServerTask = nullptr;
  ss << DISCOVERY_SERVER_ENDPOINT << ":" << m_pOpcUaServer->getPort();

  auto it = m_serverUrl.find(serverName);
  if (it != m_serverUrl.end()) {
    // Check if pointer is valid
    std::cout << "found Url in Cache" << std::endl;
    return it->second;
  }

  while (!m_pOpcUaServer->isRunning()) {
    std::cout << "Waiting for OPC UA-Server" << std::endl;
    std::this_thread::yield();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }



  auto pClient = std::shared_ptr<UA_Client>(UA_Client_new(), UA_Client_delete);
  UA_ClientConfig_setDefault(UA_Client_getConfig(pClient.get()));

  std::string serverUrl;
  std::string discoveryServerEndpoint = "";

  do{
    auto knownServer = m_pOpcUaServer->getKnownServer();
    for (auto &server: knownServer) {
      std::cout << "Check DiscoveryServer " << server << " for ServerName: " << serverName << std::endl;
      changeLocalHostname(server);
      serverUrl = getServerUrlFromMdnsLDS(serverName, server, pClient);
      if (!serverUrl.empty()) {
        m_serverUrl.insert(std::make_pair(serverName, serverUrl));
        break; // use the first URL found;
      }
    }

    // Do not wait when a server is found
    if (!serverUrl.empty()) {
      /// \todo wait until knownServer has an update, f.e. use an Id which is incrementing each time
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
  }while (serverUrl.empty());

  if (serverUrl.empty()) throw SeRoNet::Exceptions::SeRoNetSDKException(("No Server with name: %s found!", serverName));

  return serverUrl;
}

std::string NamingServiceOpcUa::getServerUrlFromMdnsLDS(const std::string &serverName,
                                                        const std::string &discoveryServerEndpoint,
                                                        const std::shared_ptr<UA_Client> &pClient) const {
  // Use UA_Client_findServers and UA_Client_findServersOnNetwork to get all serves
  std::string serverUrl;
  //<ServerName, DiscoveryEndpointUrls>
  std::multimap<std::string, std::string> discoveryEndpointsByServerName;
  UA_ServerOnNetwork *serverOnNetwork = nullptr;
  UA_StatusCode retval;
  size_t serverOnNetworkSize = 0;
  retval = UA_Client_findServersOnNetwork(pClient.get(),
                                          discoveryServerEndpoint.c_str(),
                                          0,
                                          0,
                                          0,
                                          nullptr,
                                          &serverOnNetworkSize,
                                          &serverOnNetwork);

  if (retval != UA_STATUSCODE_GOOD) {
    open62541::Exceptions::OpcUaErrorException(retval, "in function UA_Client_findServersOnNetwork");
  }

  for (int i = 0; i < serverOnNetworkSize; ++i) {
    std::string foundServerName = getServerNameFromServerOnNetworkElement(&(serverOnNetwork[i]));
    std::cout << "ServerName: " << foundServerName << std::endl;
    std::string discoveryEndpoint = static_cast<std::string>(open62541::UA_String(&(serverOnNetwork[i].discoveryUrl)));
    discoveryEndpointsByServerName.insert(std::make_pair(foundServerName, discoveryEndpoint));
  }

  UA_Array_delete(serverOnNetwork, serverOnNetworkSize, &UA_TYPES[UA_TYPES_SERVERONNETWORK]);

  size_t registeredServerSize = 0;
  UA_ApplicationDescription *pAppDescription = nullptr;
  retval = UA_Client_findServers(pClient.get(),
                                 discoveryServerEndpoint.c_str(),
                                 0,
                                 nullptr,
                                 0,
                                 nullptr,
                                 &registeredServerSize,
                                 &pAppDescription);

  if (retval != UA_STATUSCODE_GOOD) {
    open62541::Exceptions::OpcUaErrorException(retval, "in function UA_Client_findServers");
  }

  for (int i = 0; i < registeredServerSize; ++i) {
    std::string appName = static_cast<std::string>(open62541::UA_String(&(pAppDescription[i].applicationName.text)));
    for(int iDiscoUrl = 0; iDiscoUrl < pAppDescription[i].discoveryUrlsSize; ++iDiscoUrl)
    {
      std::string discoveryEndpoint = static_cast<std::string>(open62541::UA_String(&(pAppDescription[i].discoveryUrls[iDiscoUrl])));
      discoveryEndpointsByServerName.insert(std::make_pair(appName, discoveryEndpoint));
    }
  }
  UA_Array_delete(pAppDescription, registeredServerSize, &UA_TYPES[UA_TYPES_APPLICATIONDESCRIPTION]);
  serverUrl = findServerUrlFromServerOnNetwork(serverName, discoveryEndpointsByServerName, pClient);

  return serverUrl;
}

std::string NamingServiceOpcUa::findServerUrlFromServerOnNetwork(const std::string &serverName,
                                                                 std::multimap<std::string, std::string> discoveryEndpointsByServerName,
                                                                 const std::shared_ptr<UA_Client> &pClient
) const {
  UA_StatusCode retval;
  std::string serverUrl;

  auto itPair = discoveryEndpointsByServerName.equal_range(serverName);

  for (auto it = itPair.first; it != itPair.second; ++it) {

    std::string discoveryUrl = it->second;
    changeLocalHostname(discoveryUrl);

    try {
      serverUrl = getSessionEndpointUrlFromDiscoveryEndpointUrl(pClient, discoveryUrl);
      if (!serverUrl.empty()) {
        return serverUrl;
      }
    } catch (const open62541::Exceptions::OpcUaErrorException &e) {
      if (e.StatusCode == UA_STATUSCODE_BADCONNECTIONCLOSED || e.StatusCode == UA_STATUSCODE_GOODNONCRITICALTIMEOUT) {
        //@todo remove url from Array of LDS
        std::cout << "Server  coud not conntected: " << e.what() << std::endl;
        continue;
      } else {
        throw;
      }
    }
    catch (const Exceptions::SeRoNetSDKException &e) {
      std::cout << "Server not found" << std::endl;
      continue;
    }
  }

  return serverUrl;
}

std::string NamingServiceOpcUa::getServerNameFromServerOnNetworkElement(const UA_ServerOnNetwork *Element) const {
  /// \TODO handle case when hostname contains a '-'
  /// \TODO check specification for correct format, known formats: 'UaServerCpp@vmc-a07', 'Test_26NamingService_connect_Test_Server-localhost'
  auto tmpServerName =
      static_cast<std::string>(open62541::UA_String(&Element->serverName));
  auto pos_split = tmpServerName.find_last_of("-");
  tmpServerName = tmpServerName.substr(0, pos_split);
  return tmpServerName;
}

std::string NamingServiceOpcUa::getSessionEndpointUrlFromDiscoveryEndpointUrl(const std::shared_ptr<UA_Client> &pClient,
                                                                              const std::string &discoveryUrl) const {
  std::string serverUrl = "";
  UA_EndpointDescription *endpointArray = nullptr;
  size_t endpointArraySize = 0;
  UA_StatusCode
      retval = UA_Client_getEndpoints(pClient.get(), discoveryUrl.c_str(), &endpointArraySize, &endpointArray);
  if (retval != UA_STATUSCODE_GOOD) throw open62541::Exceptions::OpcUaErrorException(retval);

  if (endpointArraySize) {
    serverUrl = static_cast<std::string>(open62541::UA_String(&endpointArray[0].endpointUrl));
  }
  /// \TODO Select best endpoint (e.g. by security)
  for (size_t j = 0; j < endpointArraySize; j++) {
    //serverUrl = static_cast<std::string>(open62541::UA_String(&endpointArray[j].endpointUrl));
    std::cout << "Found endpoint: "
              << static_cast<std::string>(open62541::UA_String(&endpointArray[j].endpointUrl)) << std::endl;
  }

  UA_Array_delete(endpointArray, endpointArraySize, &UA_TYPES[UA_TYPES_ENDPOINTDESCRIPTION]);
  return serverUrl;
}

void NamingServiceOpcUa::changeLocalHostname(std::string &discoveryUrl) const {// FIXME hack to connect to the localhost adress
  /// Not compatible to IPv6 Without Port postfix!
  std::size_t found = discoveryUrl.find("169.254.");
  if (found != std::string::npos) {
    std::size_t found_port = discoveryUrl.find_last_of(":");
    if (found_port == std::string::npos || found_port == 7) {
      found_port = discoveryUrl.size(); // No Port availiable
    }
    discoveryUrl.replace(found, found_port - found, "localhost");
  }
}

open62541::UA_NodeId NamingServiceOpcUa::createNodeId(const std::string &service, UA_UInt16 nsIndex) {
  std::stringstream ss;
  ss << "85." << service;
  return open62541::UA_NodeId(nsIndex, ss.str());
}

NamingServiceOpcUa::NamingServiceOpcUa(std::shared_ptr<SeRoNet::OPCUA::Server::OpcUaServer> pOpcUaServer)
    : m_pOpcUaServer(pOpcUaServer) {

}

}
}
}
