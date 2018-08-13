///
/// \file SendClientOpcUa.hpp
/// \author Christian von Arnim, Sebastian Friedl
/// \date 17.04.2018
///
#pragma once

#include "../../../../SmartSoftComponentDeveloperAPIcpp/SmartSoft_CD_API/smartISendClientPattern_T.h"
#include "UaClientWithMutex.hpp"
#include "Converter/CommObjectToUaVariantArray.hpp"
#include "../../CommunicationObjects/ICommunicationObject.hpp"
#include "../../CommunicationObjects/Description/SelfDescription.hpp"
#include "NamingServiceOpcUa.hpp"
#include "AsyncAnswerFactoryWithCommObject.hpp"

#include <Open62541Cpp/UA_NodeId.hpp>
#include <open62541/open62541.h>

namespace SeRoNet {
namespace OPCUA {
namespace Client {

template<class DataType>
class SendClient :
    public Smart::ISendClientPattern<DataType> {
 public:

  SendClient(Smart::IComponent *pComponent);
  ~SendClient() = default;

  Smart::StatusCode connect(const std::string &server, const std::string &service) override;

  Smart::StatusCode disconnect() override {
    return Smart::SMART_OK;
  }
  Smart::StatusCode blocking(const bool blocking) override {
    return Smart::SMART_OK;
  }
  Smart::StatusCode send(const DataType &data);

 protected:
  SeRoNet::OPCUA::Client::UaClientWithMutex_t::shpType m_pUaClientWithMutex;
  std::shared_ptr<SeRoNet::OPCUA::Client::NamingServiceOpcUa> m_namingService;
  open62541::UA_NodeId m_methodNodeId;
  open62541::UA_NodeId m_objNodeId;
  std::shared_ptr<AsyncAnswerFactoryWithCommObject<void *, DataType>> m_Factory;
  open62541::UA_NodeId m_methodId;
};

template<class DataType>
SendClient<DataType>::SendClient(Smart::IComponent *pComponent)
    : Smart::ISendClientPattern<DataType>::ISendClientPattern(pComponent),
      m_namingService(std::make_shared<SeRoNet::OPCUA::Client::NamingServiceOpcUa>()),
      m_pUaClientWithMutex(nullptr) {
}

template<class DataType>
Smart::StatusCode SendClient<DataType>::connect(const std::string &server,
                                                const std::string &service) {
  auto retValue = this->m_namingService->getConnectionAndNodeIdByName(server, service);
  m_pUaClientWithMutex = retValue.connection;
  m_methodId = retValue.nodeId;
  //FIXME use new Constructor for factory with only one element SeRoNet::OPCUA::Client::UaClientWithMutex_t
  m_Factory = std::make_shared<AsyncAnswerFactoryWithCommObject<void *, DataType>>
      (m_pUaClientWithMutex->pClient, m_pUaClientWithMutex->opcuaMutex, m_methodNodeId);
  return Smart::SMART_OK;
}

template<class DataType>
Smart::StatusCode SendClient<DataType>::send(const DataType &data) {
  typename QClientOPCUA<DataType, void *>::QueryIDtype::element_type::shpAsyncAnswer_t
      ans(m_Factory->call(data));
  return ans != nullptr ? Smart::StatusCode::SMART_OK : Smart::StatusCode::SMART_ERROR;
}

} // namespace client
} // namespace OPCUA
} // namespace SeRoNet
