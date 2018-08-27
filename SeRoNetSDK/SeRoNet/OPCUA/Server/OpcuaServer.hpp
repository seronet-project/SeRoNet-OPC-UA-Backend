//
// Created by ac120371 on 27.08.2018.
//

#ifndef SERONETSDK_OPCUASERVER_HPP
#define SERONETSDK_OPCUASERVER_HPP

#include <string>
#include <open62541/open62541.h>
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

  void setServerName(const std::string &m_name) {
    OpcUaServer::m_serverName = m_name;
  }

  UA_Server *getServer() const;

  UA_UInt16 getNsIndex1() const;

  const UA_UInt16 getNsIndex0() const;

  UA_StatusCode run();

 private:

  std::string m_serverName = "";
  UA_ServerConfig *m_Config;
  UA_Server *m_Server;
  UA_Boolean m_isRunning = true;

  UA_UInt16 m_nsIndex1;
  const UA_UInt16 m_nsIndex0 = 0; /// @TODO remove nsIndex0

  OpcUaServer();
  OpcUaServer &operator=(const OpcUaServer &);

};

}
}
}
#endif //SERONETSDK_OPCUASERVER_HPP
