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
#include <open62541/client_highlevel.h>
#include <Open62541Cpp/UA_NodeId.hpp>

#include "AsyncAnswer.hpp"
#include "InstanceStorage.hpp"
#include "IAnswerState.hpp"

namespace SeRoNet {
namespace OPCUA {
namespace Client {
template<typename T_RETURN, typename... T_CAll_ARGS>
class AsyncAnswerFactory {
 public:
  AsyncAnswerFactory(std::shared_ptr<UA_Client> pClient,
                     std::mutex &opcuaThreadMutex,
                     open62541Cpp::UA_NodeId methodNodeId);
  virtual ~AsyncAnswerFactory();

  typedef std::shared_ptr<AsyncAnswer<T_RETURN>> CallReturn_t;
  virtual CallReturn_t call(T_CAll_ARGS... args) = 0;

  void disableBlocking();
  void enableBlocking();
  bool isBlockingEnabled() { return m_blockingEnabled; }
  /// Check if the client is connected and method is available
  bool isAvailable();
 protected:
  std::shared_ptr<UA_Client> m_pClient;
  std::mutex &m_opcuaThreadMutex;
  InstanceStorage<IAnswerState> m_activeInstances;
  open62541Cpp::UA_NodeId m_methodNodeId;

  /// Store the answerState state for newly created Instances
  /// \todo move m_activeInstances, m_blockingEnabled, disableBlocking and enableBlocking to base class
  std::atomic_bool m_blockingEnabled = {true};
};

template<typename T_RETURN, typename... T_CAll_ARGS>
inline AsyncAnswerFactory<T_RETURN, T_CAll_ARGS...>::AsyncAnswerFactory(std::shared_ptr<UA_Client> pClient,
                                                                        std::mutex &opcuaThreadMutex,
                                                                        open62541Cpp::UA_NodeId methodNodeId) :
    m_pClient(pClient), m_opcuaThreadMutex(opcuaThreadMutex), m_methodNodeId(methodNodeId) {
}

template<typename T_RETURN, typename... T_CAll_ARGS>
inline AsyncAnswerFactory<T_RETURN, T_CAll_ARGS...>::~AsyncAnswerFactory() {
  /// \todo Assert instead of output
  if (!m_activeInstances.empty()) {
    printf("ERROR: Instance Storage is not empty : %s", __FUNCTION__);
    auto callb = [](IAnswerState *answerState) -> void { answerState->disconnect(); answerState->detach(); };
    m_activeInstances.foreach(callb);
  }
}

template<typename T_RETURN, typename... T_CAll_ARGS>
void AsyncAnswerFactory<T_RETURN, T_CAll_ARGS...>::disableBlocking() {
  m_blockingEnabled = false;
  auto callb = [](IAnswerState *answerState) -> void { answerState->disableBlocking(); };
  m_activeInstances.foreach(callb);
}

template<typename T_RETURN, typename... T_CAll_ARGS>
void AsyncAnswerFactory<T_RETURN, T_CAll_ARGS...>::enableBlocking() {
  m_blockingEnabled = true;
  auto callb = [](IAnswerState *answerState) -> void { answerState->enableBlocking(); };
  m_activeInstances.foreach(callb);
}

template<typename T_RETURN, typename... T_CAll_ARGS>
bool AsyncAnswerFactory<T_RETURN, T_CAll_ARGS...>::isAvailable() {
  if(!m_pClient)
  {
    return false;
  }
  {
    std::unique_lock<std::mutex> l_opcUA(this->m_opcuaThreadMutex);
    UA_Boolean executeable;
    UA_StatusCode status = UA_Client_readExecutableAttribute(m_pClient.get(), *m_methodNodeId.NodeId, &executeable);
    return status == UA_STATUSCODE_GOOD;
  }
}


}//  namespace Client
}//  namespace OPCUA
}//  namespace SeRoNet
