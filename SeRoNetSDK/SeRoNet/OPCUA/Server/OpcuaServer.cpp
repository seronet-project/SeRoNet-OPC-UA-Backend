/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#include <open62541/server.h>
#include <open62541/server_config_default.h>
#include <iostream>
#include <Open62541Cpp/UA_String.hpp>
#include <future>
#include "OpcuaServer.hpp"
#include <sstream>
#include <random>

namespace SeRoNet {
namespace OPCUA {
namespace Server {

UA_UInt16 OpcUaServer::getNsIndex1() const {
  return m_nsIndex1;
}

const UA_UInt16 OpcUaServer::getNsIndex0() const {
  return m_nsIndex0;
}

OpcUaServer::~OpcUaServer() {
  UA_Server_run_shutdown(m_pServer);
  UA_Server_delete(m_pServer);
}

void OpcUaServer::run() {

  // @todo check if we can run the server in a background task
  assert(m_pServer);
  m_isRunning = true;
  while(m_isRunning)
  {
    UA_Server_run_iterate(m_pServer, true);
  }
}

UA_Server *OpcUaServer::getServer() const {
  assert(m_pServer);
  return m_pServer;
}
void OpcUaServer::initServer(const std::string &serverName) {
  if (m_initialized) {
    std::cerr << "Server already initialized, skipped." << std::endl;
    return;
  }
  m_serverName = serverName;

  std::string seed_str(m_serverName);
  seed_str += time(nullptr);
  std::seed_seq seed(seed_str.begin(), seed_str.end());
  std::minstd_rand0 randGen(seed);
  m_port = (UA_UInt16) (49151 + (randGen() % (65535 - 49151))); // TODO better way to get a free port

  std::cout << "Port: " << m_port << std::endl;


  m_pServer = UA_Server_new(); //TODO check if we can seperate setConfig and UA_Server_new
  UA_ServerConfig_setMinimal(UA_Server_getConfig(m_pServer), m_port, NULL); /// <\TODO use port == 0 for zero, how to get the assigned port later?
  auto pConfig = UA_Server_getConfig(m_pServer);

  pConfig->applicationDescription.applicationType = UA_APPLICATIONTYPE_DISCOVERYSERVER;
  UA_LocalizedText_deleteMembers(&pConfig->applicationDescription.applicationName);
  pConfig->applicationDescription.applicationName = UA_LOCALIZEDTEXT_ALLOC("", m_serverName.c_str());

  pConfig->mdnsEnabled = true;
  pConfig->mdnsConfig.mdnsServerName = UA_STRING_ALLOC(m_serverName.c_str());
  UA_String hostname = UA_STRING_ALLOC("localhost");
  pConfig->customHostname = hostname;
  UA_String *caps = UA_String_new();
  *caps = UA_STRING_ALLOC(m_serverName.c_str());
  pConfig->mdnsConfig.serverCapabilities = caps;
  pConfig->mdnsConfig.serverCapabilitiesSize = 1;
  pConfig->discoveryCleanupTimeout = 30; //s;


  // Set publishing interval
  pConfig->publishingIntervalLimits = UA_DurationRange{1, 3600.0 * 1000.0};
  pConfig->samplingIntervalLimits = UA_DurationRange{1, 3600.0 * 1000.0};

  assert(m_pServer);

  UA_Server_run_startup(m_pServer); // Must be run before UA_Server_setServerOnNetworkCallback
  UA_Server_setServerOnNetworkCallback(m_pServer, &serverOnNetworkCallback, this);

  m_nsIndex1 = UA_Server_addNamespace(m_pServer, "http://seronet-projekt.de/");

  std::stringstream ss;
  ss << "opc.tcp://localhost:" << m_port;
  m_foundServer.push_back(ss.str());

  m_initialized = true;
}
int OpcUaServer::getPort() const {
  return m_port;
}

bool OpcUaServer::getNextDiscoveryServerEndpoint(std::string &url) {

  if (m_iterator >= m_foundServer.size()) {
    std::unique_lock<std::mutex> lock(m_ldsMutex);
    while (!m_notified) {
      m_condVar.wait(lock);
    }
    m_iterator = m_foundServer.size();
    assert(m_iterator > 0);
    m_iterator--;
  }
  std::cout << "Iterator " << m_iterator << std::endl;

  url = m_foundServer[m_iterator];
  m_iterator = --m_iterator;
  assert(!url.empty());
  return true;
}

void OpcUaServer::addDiscoveryServerEndpoint(std::string url) {
  assert(!url.empty());
  std::unique_lock<std::mutex> lock(m_ldsMutex);
  m_foundServer.push_back(url);
  m_notified = true;
  m_condVar.notify_one();
}

void serverOnNetworkCallback(const UA_ServerOnNetwork *serverOnNetwork, UA_Boolean isServerAnnounce,
                             UA_Boolean isTxtReceived, void *data) {

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
  ((OpcUaServer *) data)->addDiscoveryServerEndpoint(static_cast<std::string>(tmp)); // todo setMethode implemntieren
}

std::list<std::string> OpcUaServer::getKnownServer() {
  std::unique_lock<std::mutex> lock(m_ldsMutex);
  // Copy result so the mutex can be safely released
  std::list<std::string> ret(m_foundServer.begin(), m_foundServer.end());

  return ret;
}

}
}
}
