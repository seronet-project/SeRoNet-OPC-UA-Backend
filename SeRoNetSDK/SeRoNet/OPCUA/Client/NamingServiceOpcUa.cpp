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
    std::string serverURL;
    UA_ServerOnNetwork *serverOnNetwork = NULL;
    size_t serverOnNetworkSize = 0;

    UA_Client *client = UA_Client_new(UA_ClientConfig_default);
    UA_StatusCode retval = UA_Client_findServersOnNetwork(client, DISCOVERY_SERVER_ENDPOINT, 0, 0,
                                                          0, NULL, &serverOnNetworkSize, &serverOnNetwork);
    if (retval != UA_STATUSCODE_GOOD) {
      printf("\n\terror: %s", retval);
      UA_Client_delete(client);;
    }

    // check server exist
    for (size_t i = 0; i < serverOnNetworkSize; i++) {
      UA_ServerOnNetwork *server = &serverOnNetwork[i];
      if (serverName == std::string(open62541::UA_String(&server->serverName))) {

        std::atomic_bool run = {true};
        SeRoNet::OPCUA::Client::UaClientWithMutex_t::shpType
            shpUaClientWithMutex(
            new SeRoNet::OPCUA::Client::UaClientWithMutex_t,
            std::bind(clientWithMutexDeletedCallback, std::placeholders::_1, shared_from_this())
        );

        // std::mutex OPCUA_Mutex;
        std::promise<shpUA_Client_t> promiseClient;
        std::future<shpUA_Client_t> futureClient = promiseClient.get_future();

        std::thread opcUA_Thread(opcUaBackgroudTask,
                                 this,
                                 std::move(promiseClient),
                                 &run,
                                 std::ref(shpUaClientWithMutex->opcuaMutex),
                                 std::string(open62541::UA_String(&server->serverName))
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
        connAndThread.connection = shpUaClientWithMutex;

        m_connectionCache.insert(std::make_pair(serverName, connAndThread));

        return shpUaClientWithMutex;

      }
    }

    UA_Array_delete(serverOnNetwork, serverOnNetworkSize,
                    &UA_TYPES[UA_TYPES_SERVERONNETWORK]);
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
    std::cerr << "Connection failed: " << retval << std::endl;
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