///
/// \file NamingServiceOpcUa.cpp
/// \author Christian von Arnim
/// \date 27.04.2018
///

#include <iostream>
#include <functional>
#include <open62541/open62541.h>
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

  if (m_nsIndex == UA_UINT16_MAX) {
    this->getNsIndexFromServer(serverName);
  }

  ret.nodeId = createNodeId(service, m_nsIndex);
  ret.connection = getConnectionByName(serverName);

  return ret;
}

void NamingServiceOpcUa::getNsIndexFromServer(const std::string &serverName) {
  const UA_NodeId nodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_SERVER_NAMESPACEARRAY);
  const open62541::UA_String searchNamespace{"http://seronet-projekt.de/"};
  UA_StatusCode retval;
  open62541::UA_ArrayOfVariant nsArray(1);

  UaClientWithMutex_t::shpType connection = getConnectionByName(serverName);

  std::unique_lock<decltype(connection.get()->opcuaMutex)> lock(connection.get()->opcuaMutex);
  retval = UA_Client_readValueAttribute(connection.get()->pClient.get(), nodeId, nsArray.Variants);
  if (retval != UA_STATUSCODE_GOOD) {
    //todo better error Handling
    throw SeRoNet::Exceptions::SeRoNetSDKException(
        "Error in function getNsIndexFromServer: UA_Client_readValueAttribute");
  }

  open62541::UA_Variant returnValue = nsArray[0];
  if (!UA_Variant_hasArrayType(returnValue.Variant,
                               &UA_TYPES[UA_TYPES_STRING]))
    throw SeRoNet::Exceptions::InvalidConversion("NamespaceArray is not a Array");

  for (int i = 0; i < returnValue.Variant->arrayLength; i++) {
    auto element = (UA_String *) returnValue.Variant->data;
    open62541::UA_String currentNamespace = open62541::UA_String(&element[i]);
    if (currentNamespace == searchNamespace) {
      m_nsIndex = (UA_UInt16) i;
      std::cout << "Namespace Index of " << currentNamespace << " : " << m_nsIndex << std::endl;
      break;
    }
  }
  if (m_nsIndex == -1) throw SeRoNet::Exceptions::InvalidConversion("No Namespaceindex found");

}

UaClientWithMutex_t::shpType NamingServiceOpcUa::getConnectionByName(const std::string &serverName) {

  {
    std::unique_lock<decltype(m_connectionCacheMutex)> ul_connCache;
    // Check if cache availiable
    auto it = m_connectionCache.find(serverName);
    if (it != m_connectionCache.end()) {
      // Check if pointer is valid
      ConnectionAndThread connAndThread = it->second;
      auto shpConn = connAndThread.connection.lock();
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

    ConnectionAndThread connAndThread;

    SeRoNet::OPCUA::Client::UaClientWithMutex_t::shpType
        shpUaClientWithMutex(
        new SeRoNet::OPCUA::Client::UaClientWithMutex_t,
        std::bind(clientWithMutexDeletedCallback, std::placeholders::_1, shared_from_this(), serverName)
    );
    connAndThread.connection = shpUaClientWithMutex;

    // std::mutex OPCUA_Mutex;
    std::promise<shpUA_Client_t> promiseClient;
    std::future<shpUA_Client_t> futureClient = promiseClient.get_future();

    std::thread *opcUA_Thread =
        new std::thread(
            &NamingServiceOpcUa::opcUaBackgroundTask,
            this,
            std::move(promiseClient),
            connAndThread.pRun.get(),
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
      connAndThread.thread = opcUA_Thread;
      connAndThread.connection = shpUaClientWithMutex;

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
    *(it->second.pRun) = false;
    if (it->second.thread->joinable()) {
      it->second.thread->join();
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
  auto config = UA_ClientConfig_default;
  shpUA_Client_t pClient(UA_Client_new(config), deleteUaClient);

  UA_StatusCode retVal;

  /* Connect to a server */
  retVal = UA_Client_connect(pClient.get(), serverURL.c_str());
  if (retVal != UA_STATUSCODE_GOOD) {
    std::cout << "Connection failed: " << retVal << std::endl;
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
  ss << DISCOVERY_SERVER_ENDPOINT << ":" << Server::OpcUaServer::instance().getPort();

  if (!Server::OpcUaServer::instance().isRunning()) {
    backgroundLDSServerTask = new std::thread([]() {
      Server::OpcUaServer::instance().run();
      return 0;
    });

  }
  sleep(10);

  auto pClient = std::shared_ptr<UA_Client>(UA_Client_new(UA_ClientConfig_default), UA_Client_delete);

  std::string serverUrl;

  for (auto const &discoveryServerEndpoint : Server::OpcUaServer::instance().m_foundServer) {
    auto serverEndpoint = static_cast<std::string>(discoveryServerEndpoint);
    std::cout << ("Check DiscoveryServer %s for ServerName: ", serverEndpoint) << serverName << std::endl;
    changeLocalHostname(serverEndpoint);
    serverUrl = getServerUrlFromMdnsLDS(serverName, serverEndpoint, pClient);
    if (!serverUrl.empty()) {
      break; // use the first URL found;
    }
  }

  if (serverUrl.empty()) throw SeRoNet::Exceptions::SeRoNetSDKException(("No Server with name: %s found!", serverName));

  if (backgroundLDSServerTask && backgroundLDSServerTask->joinable()) {
    SeRoNet::OPCUA::Server::OpcUaServer::instance().stopRunning();
    backgroundLDSServerTask->join();
    delete backgroundLDSServerTask;
  }

  return serverUrl;
}

std::string NamingServiceOpcUa::getServerUrlFromMdnsLDS(const std::string &serverName,
                                                        const std::string &discoveryServerEndpoint,
                                                        const std::shared_ptr<UA_Client> &pClient) const {
  std::__cxx11::string serverUrl;
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

  if (retval != UA_STATUSCODE_GOOD)
    open62541::Exceptions::OpcUaErrorException(retval,
                                               "in function Ua_Client_findServer");

  serverUrl = findServerUrlFromServerOnNetwork(serverName, serverOnNetwork, serverOnNetworkSize, pClient);
  //FIXME maybe a memory leak
  //UA_Array_delete(serverOnNetwork, serverOnNetworkSize,
  //                &UA_TYPES[UA_TYPES_APPLICATIONDESCRIPTION]);
  return serverUrl;
}

std::string NamingServiceOpcUa::findServerUrlFromServerOnNetwork(const std::string &serverName,
                                                                 const UA_ServerOnNetwork *serverOnNetwork,
                                                                 size_t serverOnNetworkSize,
                                                                 const std::shared_ptr<UA_Client> &pClient
) const {
  UA_StatusCode retval;
  std::string serverUrl;
  for (size_t i = 0; i < serverOnNetworkSize; i++) {
    std::__cxx11::string tmpServerName = getServerNameFromServerOnNetworkElement(serverName, &serverOnNetwork[i]);
    if (serverName != tmpServerName) {
      continue;
    }

    std::__cxx11::string
        discoveryUrl = static_cast<std::__cxx11::string>(open62541::UA_String(&serverOnNetwork[i].discoveryUrl));
    changeLocalHostname(discoveryUrl);
    try {
      serverUrl = getServerUrlFromLDS(pClient, serverUrl, discoveryUrl);
    } catch (const open62541::Exceptions::OpcUaErrorException &e) {
      if (e.m_statusCode == UA_STATUSCODE_BADCONNECTIONCLOSED) {
        //@todo remove url from Array of LDS
        std::cout << "Server  coud not conntected" << std::endl;
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

std::string NamingServiceOpcUa::getServerNameFromServerOnNetworkElement(
    const std::string &searchServerName,
    const UA_ServerOnNetwork *Element) const {
  auto tmpServerName =
      static_cast<std::string>(open62541::UA_String(&Element->serverName)).substr(0, searchServerName.size());
  return tmpServerName;
}

std::string &NamingServiceOpcUa::getServerUrlFromLDS(const std::shared_ptr<UA_Client> &pClient,
                                                     std::string &serverUrl,
                                                     const std::string &discoveryUrl) const {
  UA_EndpointDescription *endpointArray = nullptr;
  size_t endpointArraySize = 0;
  UA_StatusCode
      retval = UA_Client_getEndpoints(pClient.get(), discoveryUrl.c_str(), &endpointArraySize, &endpointArray);
  if (retval != UA_STATUSCODE_GOOD) throw open62541::Exceptions::OpcUaErrorException(retval);

  for (size_t j = 0; j < endpointArraySize; j++) {
    serverUrl = static_cast<std::__cxx11::string>(open62541::UA_String(&endpointArray[j].endpointUrl));
  }
  UA_Array_delete(endpointArray, endpointArraySize, &UA_TYPES[UA_TYPES_ENDPOINTDESCRIPTION]);
  return serverUrl;
}

void NamingServiceOpcUa::changeLocalHostname(std::string &discoveryUrl) const {// FIXME hack to connect to the localhost adress
  std::size_t found = discoveryUrl.find("169.254.");
  if (found != std::string::npos) {
    discoveryUrl.replace(found, 15, "localhost");
  }
}

open62541::UA_NodeId NamingServiceOpcUa::createNodeId(const std::string &service, UA_UInt16 nsIndex) {
  std::stringstream ss;
  ss << "85." << service;
  return open62541::UA_NodeId(nsIndex, ss.str());
}

}
}
}