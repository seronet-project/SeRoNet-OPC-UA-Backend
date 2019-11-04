/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#pragma once

#include <memory>
#include <open62541/client.h>
#include <Open62541Cpp/UA_NodeId.hpp>

#include "AsyncAnswer.hpp"
#include "InstanceStorage.hpp"
#include "IBlocking.hpp"

namespace SeRoNet {
namespace OPCUA {
namespace Client {
template<typename T_RETURN, typename... T_CAll_ARGS>
class AsyncAnswerFactory {
 public:
  AsyncAnswerFactory(std::shared_ptr<UA_Client> pClient,
                     std::mutex &opcuaThreadMutex,
                     open62541::UA_NodeId methodNodeId);
  virtual ~AsyncAnswerFactory();

  typedef std::shared_ptr<AsyncAnswer<T_RETURN>> CallReturn_t;
  virtual CallReturn_t call(T_CAll_ARGS... args) = 0;

  void disableBlocking();
  void enableBlocking();
  bool isBlockingEnabled() { return m_blockingEnabled; }
 protected:
  std::shared_ptr<UA_Client> m_pClient;
  std::mutex &m_opcuaThreadMutex;
  InstanceStorage<IBlocking> m_activeInstances;
  open62541::UA_NodeId m_methodNodeId;

  /// Store the blocking state for newly created Instances
  /// \todo move m_activeInstances, m_blockingEnabled, disableBlocking and enableBlocking to base class
  std::atomic_bool m_blockingEnabled = {true};
};

template<typename T_RETURN, typename... T_CAll_ARGS>
inline AsyncAnswerFactory<T_RETURN, T_CAll_ARGS...>::AsyncAnswerFactory(std::shared_ptr<UA_Client> pClient,
                                                                        std::mutex &opcuaThreadMutex,
                                                                        open62541::UA_NodeId methodNodeId) :
    m_pClient(pClient), m_opcuaThreadMutex(opcuaThreadMutex), m_methodNodeId(methodNodeId) {
}

template<typename T_RETURN, typename... T_CAll_ARGS>
inline AsyncAnswerFactory<T_RETURN, T_CAll_ARGS...>::~AsyncAnswerFactory() {
  /// \todo Assert instead of output
  if (!m_activeInstances.empty()) {
    printf("ERROR: Instance Storage is not empty : %s", __FUNCTION__);
  }
}

template<typename T_RETURN, typename... T_CAll_ARGS>
void AsyncAnswerFactory<T_RETURN, T_CAll_ARGS...>::disableBlocking() {
  m_blockingEnabled = false;
  auto callb = [](IBlocking *blocking) -> void { blocking->disableBlocking(); };
  m_activeInstances.foreach(callb);
}

template<typename T_RETURN, typename... T_CAll_ARGS>
void AsyncAnswerFactory<T_RETURN, T_CAll_ARGS...>::enableBlocking() {
  m_blockingEnabled = true;
  auto callb = [](IBlocking *blocking) -> void { blocking->enableBlocking(); };
  m_activeInstances.foreach(callb);
}

}//  namespace Client
}//  namespace OPCUA
}//  namespace SeRoNet
