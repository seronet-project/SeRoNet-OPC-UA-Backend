//
// Created by ac120371 on 27.08.2018.
//

#include "OpcuaServer.hpp"
namespace SeRoNet {
namespace OPCUA {
namespace Server {

OpcUaServer::OpcUaServer() {

  m_Config = UA_ServerConfig_new_default(); // TODO chnage Port Nr - for example

  m_Config->mdnsServerName = UA_String_fromChars(m_serverName.c_str());
  m_Config->applicationDescription.applicationName = UA_LOCALIZEDTEXT_ALLOC("en", m_serverName.c_str());
  m_Config->applicationDescription.applicationType = UA_APPLICATIONTYPE_SERVER;
  m_Config->mdnsServerName = UA_String_fromChars(m_serverName.c_str());

  UA_String *caps = UA_String_new();
  *caps = UA_String_fromChars(m_serverName.c_str());
  m_Config->serverCapabilities = caps;
  m_Server = UA_Server_new(m_Config);
  m_nsIndex1 = UA_Server_addNamespace(m_Server, "http://seronet-projekt.de/");

}

UA_UInt16 OpcUaServer::getNsIndex1() const {
  return m_nsIndex1;
}

const UA_UInt16 OpcUaServer::getNsIndex0() const {
  return m_nsIndex0;
}

OpcUaServer::~OpcUaServer() {
  UA_ServerConfig_delete(m_Config);
  UA_Server_delete(m_Server);
}

UA_StatusCode OpcUaServer::run() {
  return UA_Server_run(m_Server, &m_isRunning);
}

UA_Server *OpcUaServer::getServer() const {
  return m_Server;
}

}
}
}