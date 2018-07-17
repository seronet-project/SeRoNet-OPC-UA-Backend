///
/// \file NamingServiceOpcUa.cpp
/// \author Christian von Arnim
/// \date 27.04.2018
///

#include <iostream>
#include <functional>
#include <open62541/open62541.h>
#include <Open62541Cpp/UA_String.hpp>
#include <sstream>

#include "NamingServiceOpcUa.hpp"
#include "../../Exceptions/SeRoNetSDKException.hpp"

#define DISCOVERY_SERVER_ENDPOINT "opc.tcp://localhost:4840"

namespace SeRoNet {
namespace OPCUA {
namespace Client {

INamingService::ConnectionAndNodeid NamingServiceOpcUa::getConnectionAndNodeIdByName(const std::string &serverName,
                                                                                     const std::string &service) {

  ConnectionAndNodeid ret;
  ret.nodeId = createNodeIdFromServiceName(service);
  ret.connection = getConnectionByName(serverName);

  return ret;
}
OPEN_65241_CPP_NAMESPACE::UA_NodeId NamingServiceOpcUa::createNodeIdFromServiceName(const std::string &service) {

  std::stringstream ss;
  ss << "85." << service;
  return OPEN_65241_CPP_NAMESPACE::UA_NodeId(2, ss.str());
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

    std::string serverUrl = getEndpointByName(serverName);

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
            &NamingServiceOpcUa::opcUaBackgroudTask,
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
  if(it != pNamingService->m_connectionCache.end())
  {
    *(it->second.pRun) = false;
    if(it->second.thread->joinable())
    {
      it->second.thread->join();
    }
  }
  pNamingService->m_connectionCache.erase(it);
  delete pClientWithMutex;
}

void NamingServiceOpcUa::opcUaBackgroudTask(std::promise<shpUA_Client_t> promiseClient,
                                            std::atomic_bool *run,
                                            std::mutex &OPCUA_Mutex,
                                            std::string serverURL) {
  // Initialize shared pointer so it will clean up UA_Client*
  auto config = UA_ClientConfig_default;
  shpUA_Client_t pClient(UA_Client_new(config), deleteUaClient);

  UA_StatusCode retval;

  /* Connect to a server */
  retval = UA_Client_connect(pClient.get(), serverURL.c_str());
  if (retval != UA_STATUSCODE_GOOD) {
    std::cout << "Connection failed: " << retval << std::endl;
    pClient = nullptr;
    promiseClient.set_value(pClient);
    return;
  }
  promiseClient.set_value(pClient);
  /// OPC UA Client Loop, acquire lock while checking for new messages
  {
    /// Initialize without locking
    std::unique_lock<std::remove_reference<decltype(OPCUA_Mutex)>::type> l_opcUA(OPCUA_Mutex, std::defer_lock);
    int cnt = 0;
    while (*run) {
      l_opcUA.lock();
      UA_Client_run_iterate(pClient.get(), 10);
      l_opcUA.unlock();
      std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
  }
}
std::string NamingServiceOpcUa::getEndpointByName(const std::string serverName) {

  UA_ApplicationDescription *applicationDescriptionArray = nullptr;
  size_t applicationDescriptionArraySize = 0;

  auto pClient = std::shared_ptr<UA_Client>(UA_Client_new(UA_ClientConfig_default), UA_Client_delete);

  UA_StatusCode retval;

  retval = UA_Client_findServers(pClient.get(), DISCOVERY_SERVER_ENDPOINT, 0, nullptr, 0, nullptr,
                                 &applicationDescriptionArraySize, &applicationDescriptionArray);

  if (retval != UA_STATUSCODE_GOOD) {
    //todo better error Handling
    throw SeRoNet::Exceptions::SeRoNetSDKException("Error in function Ua_Client_findServer");
  }
  std::string serverUrl;
  for (size_t i = 0; i < applicationDescriptionArraySize; i++) {
    UA_ApplicationDescription *description = &applicationDescriptionArray[i];
    if (serverName != static_cast<std::string>(open62541::UA_String(&description->applicationName.text))) {
      continue;
    }

    UA_EndpointDescription *endpointArray = nullptr;
    size_t endpointArraySize = 0;

    std::string discoveryUrl = static_cast<std::string>(open62541::UA_String(&description->discoveryUrls[0]));
    retval = UA_Client_getEndpoints(pClient.get(), discoveryUrl.c_str(), &endpointArraySize, &endpointArray);
    if (retval != UA_STATUSCODE_GOOD) {
      break;
    }
    for (size_t j = 0; j < endpointArraySize; j++) {
      serverUrl = static_cast<std::string>(open62541::UA_String(&endpointArray[j].endpointUrl));
    }
    UA_Array_delete(endpointArray, endpointArraySize, &UA_TYPES[UA_TYPES_ENDPOINTDESCRIPTION]);
  }
  UA_Array_delete(applicationDescriptionArray, applicationDescriptionArraySize,
                  &UA_TYPES[UA_TYPES_APPLICATIONDESCRIPTION]);

  if (serverUrl.empty()) {
    throw SeRoNet::Exceptions::SeRoNetSDKException(("No Server with name: %s found!", serverName));
  }

  return serverUrl;
}

}
}
}