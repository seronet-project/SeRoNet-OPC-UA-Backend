///
/// \file SendClientOpcUa.hpp
/// \author Christian von Arnim, Sebastian Friedl
/// \date 17.04.2018
///
#pragma once

#include "../../../../SmartSoftComponentDeveloperAPIcpp/SmartSoft_CD_API/smartISendClientPattern_T.h"
#include "UaClientWithMutex.hpp"
#include "SeRoNet/OPCUA/Converter/CommObjectToUaVariantArray.hpp"
#include "../../CommunicationObjects/ICommunicationObject.hpp"
#include "../../CommunicationObjects/Description/SelfDescription.hpp"
#include "NamingServiceOpcUa.hpp"
#include "AsyncAnswerFactoryWithCommObject.hpp"
#include "WiringHandler.hpp"
#include "../Server/WiringSlave.hpp"

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
    //TODO disconnect client from Server
    return Smart::SMART_OK;
  }

  Smart::StatusCode add(Server::WiringSlave *slave, const std::string &portName);
  Smart::StatusCode remove();

  Smart::StatusCode blocking(const bool blocking) override {
    //TODO implement blocking
    return Smart::SMART_OK;
  }

  Smart::StatusCode send(const DataType &data);

 protected:

  WiringHandler<SendClient> m_wiringHandler;
  SeRoNet::OPCUA::Client::UaClientWithMutex_t::shpType m_pUaClientWithMutex;
  std::shared_ptr<SeRoNet::OPCUA::Client::NamingServiceOpcUa> m_namingService;
  open62541::UA_NodeId m_methodNodeId;
  open62541::UA_NodeId m_objNodeId;
  std::shared_ptr<AsyncAnswerFactoryWithCommObject<void *, DataType>> m_Factory;
};

template<class DataType>
SendClient<DataType>::SendClient(Smart::IComponent *pComponent)
    : Smart::ISendClientPattern<DataType>::ISendClientPattern(pComponent),
      m_namingService(std::make_shared<SeRoNet::OPCUA::Client::NamingServiceOpcUa>()),
      m_pUaClientWithMutex(nullptr),
      m_wiringHandler(this) {
  ;
}

template<class DataType>
Smart::StatusCode SendClient<DataType>::connect(const std::string &server,
                                                const std::string &service) {
  auto retValue = this->m_namingService->getConnectionAndNodeIdByName(server, service);
  m_pUaClientWithMutex = retValue.connection;
  m_methodNodeId = retValue.nodeId;
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

template<class DataType>
Smart::StatusCode SendClient<DataType>::add(Server::WiringSlave *slave, const std::string &portName) {
  return m_wiringHandler.add(slave,
                             portName,
                             std::bind(&SendClient<DataType>::connect, this,
                                       std::placeholders::_1, std::placeholders::_2),
                             std::bind(&SendClient<DataType>::disconnect, this));
}

} // namespace client
} // namespace OPCUA
} // namespace SeRoNet
