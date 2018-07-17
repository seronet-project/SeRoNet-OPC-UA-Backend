///
/// \file QClientOPCUACommObj.hpp
/// \author Christian von Arnim
/// \date 19.02.2018
///
#pragma once

#include "QClientOPCUA.hpp"
#include "NamingServiceOpcUa.hpp"
#include <open62541.h>
#include <SeRoNetSDK/SeRoNet/OPCUA/Client/AsyncAnswerFactoryWithCommObject.hpp>

namespace SeRoNet {
namespace OPCUA {
namespace Client {

template<class RequestType, class AnswerType>
class QueryClient :
    public QClientOPCUA<RequestType, AnswerType> {
 public:
  QueryClient(Smart::IComponent *pComponent);
  ~QueryClient() = default;

  Smart::StatusCode queryRequest(
      const RequestType &request,
      typename QClientOPCUA<RequestType, AnswerType>::QueryIDtype &id
  ) override;

  Smart::StatusCode blocking(const bool blocking) override;

  Smart::StatusCode connect(const std::string &server, const std::string &service) override;

 protected:
  std::shared_ptr<AsyncAnswerFactoryWithCommObject<RequestType, AnswerType>> m_Factory;
  std::shared_ptr<SeRoNet::OPCUA::Client::NamingServiceOpcUa> m_namingService;
  SeRoNet::OPCUA::Client::UaClientWithMutex_t::shpType m_pUaClientWithMutex;

};

template<class RequestType, class AnswerType>
inline QueryClient<RequestType, AnswerType>::QueryClient(
    Smart::IComponent *pComponent
) :
    QClientOPCUA<RequestType, AnswerType>::QClientOPCUA(pComponent),
    m_namingService(std::make_shared<SeRoNet::OPCUA::Client::NamingServiceOpcUa>()),
    m_pUaClientWithMutex(nullptr)
{
}

template<class RequestType, class AnswerType>
inline Smart::StatusCode QueryClient<RequestType, AnswerType>::queryRequest(
    const RequestType &request,
    typename QClientOPCUA<RequestType, AnswerType>::QueryIDtype &id
) {
  typename QClientOPCUA<RequestType, AnswerType>::QueryIDtype::element_type::shpAsyncAnswer_t
      ans(m_Factory->call(request));
  id.reset(new typename QClientOPCUA<RequestType, AnswerType>::QueryIDtype::element_type(ans));
  return ans != nullptr ? Smart::StatusCode::SMART_OK : Smart::StatusCode::SMART_ERROR;
}

template<class RequestType, class AnswerType>
inline Smart::StatusCode QueryClient<RequestType, AnswerType>::blocking(const bool blocking) {
  if (blocking) {
    m_Factory->enableBlocking();
  } else {
    m_Factory->disableBlocking();
  }

  return m_Factory->isBlockingEnabled() == blocking ? Smart::StatusCode::SMART_OK : Smart::StatusCode::SMART_ERROR;
}

template<class RequestType, class AnswerType>
Smart::StatusCode QueryClient<RequestType, AnswerType>::connect(const std::string &server,
                                                                        const std::string &service) {
  auto retValue = this->m_namingService->getConnectionAndNodeIdByName(server, service);
  m_pUaClientWithMutex = retValue.connection;
  //FIXME use new Constructor for factory with only one element SeRoNet::OPCUA::Client::UaClientWithMutex_t
  m_Factory = std::make_shared<AsyncAnswerFactoryWithCommObject<RequestType, AnswerType>>(m_pUaClientWithMutex->pClient,m_pUaClientWithMutex->opcuaMutex);
  return Smart::SMART_OK;
}

}
}
}
