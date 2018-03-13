///
/// \file AsyncAnswerFactoryWithCommObject.hpp
/// \author Christian von Arnim
/// \date 16.02.2018
///

#pragma once

#include "AsyncAnswerFactory.hpp"
#include "AsyncAnswerMethodCommObjectRequest.hpp"

namespace SeRoNet {
namespace OPCUA {
namespace Client {

template<typename T_RETURN, typename T_REQUEST>
class AsyncAnswerFactoryWithCommObject :
    public AsyncAnswerFactory<T_RETURN, T_REQUEST> {
 public:
  static_assert(
      std::is_base_of<CommunicationObjects::ICommunicationObject, T_RETURN>::value,
      "T_RETURN must be a descendant of CommunicationObjects::ICommunicationObject");

  static_assert(
      std::is_base_of<CommunicationObjects::ICommunicationObject, T_REQUEST>::value,
      "T_REQUEST must be a descendant of CommunicationObjects::ICommunicationObject");

  using AsyncAnswerFactory<T_RETURN, T_REQUEST>::AsyncAnswerFactory;
  typename AsyncAnswerFactory<T_RETURN, T_REQUEST>::CallReturn_t call(T_REQUEST arg) override {
    std::unique_lock<std::mutex> l_opcUA(this->m_opcuaThreadMutex);
    return typename AsyncAnswerFactory<T_RETURN, T_REQUEST>::CallReturn_t(
        new AsyncAnswerMethodCommObjectRequest<T_RETURN>
            (&this->m_activeInstances,
             this->m_blockingEnabled,
             this->m_pClient.get(),
             &arg));
  }
};

}//  namespace Client
}//  namespace OPCUA
}//  namespace SeRoNet
