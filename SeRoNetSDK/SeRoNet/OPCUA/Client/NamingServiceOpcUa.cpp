///
/// \file NamingServiceOpcUa.cpp
/// \author Christian von Arnim
/// \date 27.04.2018
///

#include <iostream>
#include <functional>
#include <open62541/open62541.h>
#include <Open62541Cpp/UA_String.hpp>

#include "NamingServiceOpcUa.hpp"
#include "../../Exceptions/SeRoNetSDKException.hpp"

#define DISCOVERY_SERVER_ENDPOINT "opc.tcp://localhost:4840"

namespace SeRoNet {
namespace OPCUA {
namespace Client {

INamingService::ConnectionAndNodeid NamingServiceOpcUa::getConnectionAndNodeIdByName(const std::string &serverName,
                                                                                     const std::string &service) {

  ConnectionAndNodeid ret;
  ret.connection = getConnectionByName(serverName);

  return ret;
}

UaClientWithMutex_t::shpType NamingServiceOpcUa::getConnectionByName(const std::string &serverName) {

  UA_Client *client = UA_Client_new(UA_ClientConfig_default);
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
    // No connection in the cache, create a new one
  } else {

    ///@todo Aquirre server url ("opc.tcp://host:port")

    UA_ApplicationDescription *applicationDescriptionArray = nullptr;
    size_t applicationDescriptionArraySize = 0;

    UA_StatusCode retval;
    {

      retval = UA_Client_findServers(client, DISCOVERY_SERVER_ENDPOINT, 0, nullptr, 0, nullptr,
                                     &applicationDescriptionArraySize, &applicationDescriptionArray);
    }

    if (retval != UA_STATUSCODE_GOOD) {
      //todo better error Handling
      throw SeRoNet::Exceptions::SeRoNetSDKException("Error in function Ua_Client_findServer");

    }

    // check server exist
    for (size_t i = 0; i < applicationDescriptionArraySize; i++) {
      UA_ApplicationDescription *description = &applicationDescriptionArray[i];
      if (serverName == std::string(open62541::UA_String(&description->applicationName.text))) {

        std::atomic_bool run = {true};
        SeRoNet::OPCUA::Client::UaClientWithMutex_t::shpType
            shpUaClientWithMutex(
            new SeRoNet::OPCUA::Client::UaClientWithMutex_t,
            std::bind(clientWithMutexDeletedCallback, std::placeholders::_1, shared_from_this())
        );

        UA_EndpointDescription *endpointArray = nullptr;
        size_t endpointArraySize = 0;
        std::string serverUrl;
        std::string discoveryUrl = std::string(open62541::UA_String(&description->discoveryUrls[0]));
        retval = UA_Client_getEndpoints(client, discoveryUrl.c_str(), &endpointArraySize, &endpointArray);
        if (retval != UA_STATUSCODE_GOOD) {
          break;
        }
        for (size_t j = 0; j < endpointArraySize; j++) {
          serverUrl = std::string(open62541::UA_String(&endpointArray[j].endpointUrl));
        }
        // std::mutex OPCUA_Mutex;
        std::promise<shpUA_Client_t> promiseClient;
        std::future<shpUA_Client_t> futureClient = promiseClient.get_future();

        std::thread *opcUA_Thread = new std::thread(opcUaBackgroudTask,
                                                    this,
                                                    std::move(promiseClient),
                                                    &run,
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
        ConnectionAndThread connAndThread;
        connAndThread.thread = opcUA_Thread;
        connAndThread.connection = shpUaClientWithMutex;

        m_connectionCache.insert(std::make_pair(serverName, connAndThread));

        return shpUaClientWithMutex;

      }
    }

    UA_Array_delete(applicationDescriptionArray, applicationDescriptionArraySize,
                    &UA_TYPES[UA_TYPES_APPLICATIONDESCRIPTION]);
    throw SeRoNet::Exceptions::SeRoNetSDKException(("No Server with name: %s found!",serverName));
  }
}

void deleteUaClient(UA_Client *pClient) {
  if (pClient != nullptr) {
    UA_Client_disconnect(pClient);
    UA_Client_delete(pClient);
  }
}

void NamingServiceOpcUa::clientWithMutexDeletedCallback(UaClientWithMutex_t *pClientWithMutex,
                                                        NamingServiceOpcUa::shpType pNamingService) {
  /// \todo Kill the worker thread of the UaClient

  delete pClientWithMutex;
}

void NamingServiceOpcUa::opcUaBackgroudTask(std::promise<shpUA_Client_t> promiseClient,
                                            std::atomic_bool *run,
                                            std::mutex &OPCUA_Mutex,
                                            std::string serverURL) {
  // Initialize shared pointer so it will clean up UA_Client*
  auto config = UA_ClientConfig_default;
  config.localConnectionConfig.useBlockingSocket = false;
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
    std::this_thread::sleep_for(std::chrono::seconds(5));

    /// Initialize without locking
    std::unique_lock<std::remove_reference<decltype(OPCUA_Mutex)>::type> l_opcUA(OPCUA_Mutex, std::defer_lock);
    int cnt = 0;
    while (*run) {
      l_opcUA.lock();
      UA_Client_runAsync(pClient.get(), 10);
      if (++cnt == 10) {
        cnt = 0;
        /// HACK Find out how to "store" PublishRequests on the server side, or register a callback for empty responses
        UA_Client_Subscriptions_manuallySendPublishRequest_Async(pClient.get());
      }
      l_opcUA.unlock();
      std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
  }
}

}
}
}