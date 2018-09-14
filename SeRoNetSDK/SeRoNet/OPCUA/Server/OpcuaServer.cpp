//
// Created by ac120371 on 27.08.2018.
//

#include <open62541/open62541.h>
#include <iostream>
#include <Open62541Cpp/UA_String.hpp>
#include "OpcuaServer.hpp"

namespace SeRoNet {
namespace OPCUA {
namespace Server {

OpcUaServer::OpcUaServer() = default;

UA_UInt16 OpcUaServer::getNsIndex1() const {
  return m_nsIndex1;
}

const UA_UInt16 OpcUaServer::getNsIndex0() const {
  return m_nsIndex0;
}

OpcUaServer::~OpcUaServer() {
  UA_ServerConfig_delete(m_Config);
  //UA_Server_delete(m_Server);
}

UA_StatusCode OpcUaServer::run() {
  assert(m_Server);
  m_isRunning = true;
  return UA_Server_run(m_Server, &m_isRunning);
}

UA_Server *OpcUaServer::getServer() const {
  assert(m_Server);
  return m_Server;
}
void OpcUaServer::initServer(const std::string &m_name) {
  srand((unsigned) time(nullptr)); // Zufallsgenerator initialisieren.
  m_port = (UA_UInt16) (49151 + (std::rand() % (65535 - 49151))); // TODO better way to get a free port

  m_Config = UA_ServerConfig_new_minimal(m_port, NULL);

  m_Config->applicationDescription.applicationType = UA_APPLICATIONTYPE_DISCOVERYSERVER;
  m_serverName = m_name;
  m_Config->mdnsServerName = UA_String_fromChars(m_serverName.c_str());
  m_Config->applicationDescription.applicationName = UA_LOCALIZEDTEXT_ALLOC("", m_serverName.c_str());
  m_Config->mdnsServerName = UA_String_fromChars(m_serverName.c_str());
  UA_String hostname = UA_String_fromChars("localhost");
  m_Config->customHostname = hostname;
  UA_String *caps = UA_String_new();
  *caps = UA_String_fromChars(m_serverName.c_str());
  m_Config->serverCapabilities = caps;
  m_Config->discoveryCleanupTimeout = 30; //s;

  m_Server = UA_Server_new(m_Config); //TODO check if we can seperate setConfig and UA_Server_new
  assert(m_Server);
  UA_Server_setServerOnNetworkCallback(m_Server, &serverOnNetworkCallback, nullptr);

  m_nsIndex1 = UA_Server_addNamespace(m_Server, "http://seronet-projekt.de/");
}
int OpcUaServer::getPort() const {
  return m_port;
}

void serverOnNetworkCallback(const UA_ServerOnNetwork *serverOnNetwork,
                             UA_Boolean isServerAnnounce,
                             UA_Boolean isTxtReceived,
                             void */*data*/) {

  if (!isServerAnnounce) {
    std::cout << (
        "serverOnNetworkCallback called, but discovery URL "
        "already initialized or is not announcing. Ignoring.");
    return; // we already have everything we need or we only want server announces
  }

  if (!isTxtReceived) return; // we wait until the corresponding TXT record is announced.
  // Problem: how to handle if a Server does not announce the
  // optional TXT?

  // here you can filter for a specific LDS server, e.g. call FindServers on
  // the serverOnNetwork to make sure you are registering with the correct
  // LDS. We will ignore this for now
  //std::cout << ( "Another server announced itself on %.*s",
  //            (int)serverOnNetwork->discoveryUrl.length, serverOnNetwork->discoveryUrl.data);
  open62541::UA_String tmp = open62541::UA_String(&serverOnNetwork->discoveryUrl);
  OpcUaServer::instance().m_foundServer.push_back(tmp);
}

std::string OpcUaServer::getDiscoveryServerEndpoint() {
  //FIXME use std::future instead
  assert(!static_cast<std::string>(m_foundServer[0]).empty());

  return static_cast<std::string>(m_foundServer[0]);
}

}
}
}
