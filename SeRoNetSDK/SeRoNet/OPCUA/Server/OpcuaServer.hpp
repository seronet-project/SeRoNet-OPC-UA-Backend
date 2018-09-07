//
// Created by ac120371 on 27.08.2018.
//

#ifndef SERONETSDK_OPCUASERVER_HPP
#define SERONETSDK_OPCUASERVER_HPP

#include <string>
#include <open62541/open62541.h>
#include <Open62541Cpp/UA_String.hpp>
#include <vector>
namespace SeRoNet {
namespace OPCUA {
namespace Server {

/// Desin Pattern Singeton
class OpcUaServer {
 public:
  static OpcUaServer &instance() {
    static OpcUaServer _instance;
    return _instance;
  }


  virtual ~OpcUaServer();

  const std::string &getServerName() const {
    return m_serverName;
  }

  void initServer(const std::string &m_name);

  UA_Server *getServer() const;

  UA_UInt16 getNsIndex1() const;

  const UA_UInt16 getNsIndex0() const;

  UA_StatusCode run();

  bool isRunning() { return m_isRunning; };

  void stopRunning() {
    m_isRunning = false; // TODO check if all server should be stopping
  }

  int getPort() const;

  std::string getDiscoveryServerEndpoint();

  friend void serverOnNetworkCallback(const UA_ServerOnNetwork *serverOnNetwork, UA_Boolean isServerAnnounce,
                                      UA_Boolean isTxtReceived, void *data);
  std::vector<open62541::UA_String> m_foundServer; // TODO better lock the vector

 private:


  std::string m_serverName = "";
  UA_ServerConfig *m_Config;
  UA_Server *m_Server;
  UA_Boolean m_isRunning = false;

  UA_UInt16 m_nsIndex1;
  const UA_UInt16 m_nsIndex0 = 0;

  OpcUaServer();
  OpcUaServer &operator=(const OpcUaServer &);

  UA_UInt16 m_port;

};

void serverOnNetworkCallback(const UA_ServerOnNetwork *serverOnNetwork, UA_Boolean isServerAnnounce,
                             UA_Boolean isTxtReceived, void *data);



}
}
}
#endif //SERONETSDK_OPCUASERVER_HPP
