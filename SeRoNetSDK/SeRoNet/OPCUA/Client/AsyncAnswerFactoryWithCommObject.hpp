///
/// \file AsyncAnswerFactoryWithCommObject.hpp
/// \author Christian von Arnim
/// \date 16.02.2018
///

#pragma once

#include "AsyncAnswerFactory.hpp"
#include "AsyncAnswerMethodCommObjectRequest.hpp"
#include "../../CommunicationObjects/Description/SelfDescription.hpp"

namespace SeRoNet {
namespace OPCUA {
namespace Client {

template<typename T_RETURN, typename T_REQUEST>
class AsyncAnswerFactoryWithCommObject :
    public AsyncAnswerFactory<T_RETURN, T_REQUEST> {
 public:

  using AsyncAnswerFactory<T_RETURN, T_REQUEST>::AsyncAnswerFactory;
  typename AsyncAnswerFactory<T_RETURN, T_REQUEST>::CallReturn_t call(T_REQUEST arg) override {
    std::unique_lock<std::mutex> l_opcUA(this->m_opcuaThreadMutex);
    return typename AsyncAnswerFactory<T_RETURN, T_REQUEST>::CallReturn_t(
        new AsyncAnswerMethodCommObjectRequest<T_RETURN>
            (&this->m_activeInstances,
             this->m_blockingEnabled,
             this->m_pClient.get(),
             CommunicationObjects::Description::SelfDescription(&arg),
             this->m_methodNodeId));
  }
};

}//  namespace Client
}//  namespace OPCUA
}//  namespace SeRoNet
