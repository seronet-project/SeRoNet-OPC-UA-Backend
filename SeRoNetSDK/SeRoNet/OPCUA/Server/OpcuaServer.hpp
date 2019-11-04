/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#ifndef SERONETSDK_OPCUASERVER_HPP
#define SERONETSDK_OPCUASERVER_HPP

#include "../../../Definitions.hpp"

#include <string>
#include <open62541/server.h>
#include <Open62541Cpp/UA_String.hpp>
#include <vector>
#include <condition_variable>
#include <mutex>
#include <list>
#include <thread>

namespace SeRoNet {
namespace OPCUA {
namespace Server {

/// Desin Pattern Singeton
class SERONETSDK_EXPORT OpcUaServer {
 public:
  // TODO Remove
  /*
  static OpcUaServer &instance() {
    static OpcUaServer _instance;
    return _instance;
  }
  */

  virtual ~OpcUaServer();

  const std::string &getServerName() const {
    return m_serverName;
  }

  void initServer(const std::string &serverName);

  UA_Server *getServer() const;

  UA_UInt16 getNsIndex1() const;

  const UA_UInt16 getNsIndex0() const;

  void run();

  bool isRunning() { return m_isRunning; };

  void stopRunning() {
    m_isRunning = false; // TODO check if all server should be stopping
    std::this_thread::yield();
  }

  int getPort() const;

  /// \deprecated Poll getKnownServer() instead
  /// \bug multiple requests are causing huge lag
  /// \TODO define an altenative blocking function for this with timeout
  bool getNextDiscoveryServerEndpoint(std::string &url);

  void addDiscoveryServerEndpoint(std::string url);

  friend void serverOnNetworkCallback(const UA_ServerOnNetwork *serverOnNetwork, UA_Boolean isServerAnnounce,
                                      UA_Boolean isTxtReceived, void *data);

  std::list<std::string> getKnownServer();
 private:

  bool m_initialized = false;

  unsigned int m_iterator = {0};
  // TODO use std::set
  std::vector<std::string> m_foundServer;
  std::mutex m_ldsMutex;
  std::condition_variable m_condVar;
  bool m_notified = false;

  std::string m_serverName = "";

  UA_Server *m_pServer = nullptr;
  UA_Boolean m_isRunning = false;

  UA_UInt16 m_nsIndex1;
  const UA_UInt16 m_nsIndex0 = 0;

  OpcUaServer &operator=(const OpcUaServer &);

  UA_UInt16 m_port;

};

void serverOnNetworkCallback(const UA_ServerOnNetwork *serverOnNetwork, UA_Boolean isServerAnnounce,
                             UA_Boolean isTxtReceived, void *data);






}
}
}
#endif //SERONETSDK_OPCUASERVER_HPP
