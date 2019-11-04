/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

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
             CommunicationObjects::Description::SelfDescription(&arg, ""),
             this->m_methodNodeId));
  }
};

}//  namespace Client
}//  namespace OPCUA
}//  namespace SeRoNet
