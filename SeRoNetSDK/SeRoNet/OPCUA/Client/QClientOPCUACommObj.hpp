///
/// \file QClientOPCUACommObj.hpp
/// \author Christian von Arnim
/// \date 19.02.2018
///
#pragma once

#include "QClientOPCUA.hpp"
#include <open62541.h>
#include <SeRoNetSDK/SeRoNet/OPCUA/Client/AsyncAnswerFactoryWithCommObject.hpp>

namespace SeRoNet {
namespace OPCUA {
namespace Client {

template<class RequestType, class AnswerType>
class QClientOPCUACommObj :
    public QClientOPCUA<RequestType, AnswerType> {
 public:
  QClientOPCUACommObj(Smart::IComponent *pComponent, std::shared_ptr<UA_Client> pClient, std::mutex &opcuaThreadMutex);
  ~QClientOPCUACommObj() = default;

  Smart::StatusCode queryRequest(
      const RequestType &request,
      typename QClientOPCUA<RequestType, AnswerType>::QueryIDtype &id
  ) override;
  Smart::StatusCode blocking(const bool blocking) override;
 protected:
  AsyncAnswerFactoryWithCommObject<RequestType, AnswerType> m_Factory;
};

template<class RequestType, class AnswerType>
inline QClientOPCUACommObj<RequestType, AnswerType>::QClientOPCUACommObj(
    Smart::IComponent *pComponent,
    std::shared_ptr<UA_Client> pClient,
    std::mutex &opcuaThreadMutex
) : m_Factory(pClient, opcuaThreadMutex), QClientOPCUA<RequestType, AnswerType>::QClientOPCUA(pComponent) {
}

template<class RequestType, class AnswerType>
inline Smart::StatusCode QClientOPCUACommObj<RequestType, AnswerType>::queryRequest(
    const RequestType &request,
    typename QClientOPCUA<RequestType, AnswerType>::QueryIDtype &id
) {
  typename QClientOPCUA<RequestType, AnswerType>::QueryIDtype::element_type::shpAsyncAnswer_t ans(m_Factory.call(request));
  id.reset(new typename QClientOPCUA<RequestType, AnswerType>::QueryIDtype::element_type(ans));
  return ans != nullptr ? Smart::StatusCode::SMART_OK : Smart::StatusCode::SMART_ERROR;
}

template<class RequestType, class AnswerType>
inline Smart::StatusCode QClientOPCUACommObj<RequestType, AnswerType>::blocking(const bool blocking) {
  if (blocking) {
    m_Factory.enableBlocking();
  } else {
    m_Factory.disableBlocking();
  }

  return m_Factory.isBlockingEnabled() == blocking ? Smart::StatusCode::SMART_OK : Smart::StatusCode::SMART_ERROR;
}

}
}
}
