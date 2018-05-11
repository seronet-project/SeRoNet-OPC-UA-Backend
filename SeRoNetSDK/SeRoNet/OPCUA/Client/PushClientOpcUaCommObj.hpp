///
/// @file PushClientOpcUa.hpp
/// @author Christian von Arnim 
/// @date 08.01.2018
///

#pragma once

#include "AsyncSubscriptionOpcUa.hpp"
#include "AsyncSubscriptionReader.hpp"
#include "../../../../SmartSoftComponentDeveloperAPIcpp/SmartSoft_CD_API/smartIPushClientPattern_T.h"
#include "AsyncSubscriptionWithCommObject.hpp"

#include <open62541.h>
#include <memory>
#include "UaClientWithMutex.hpp"
#include "NamingServiceOpcUa.hpp"

namespace SeRoNet {
namespace OPCUA {
namespace Client {

template<class DataType>
class PushClientOpcUa :
    public Smart::IPushClientPattern<DataType> {
 public:
  explicit PushClientOpcUa(Smart::IComponent *pComponent,
                           open62541::UA_NodeId startNodeId);
  Smart::StatusCode connect(const std::string &server, const std::string &service) override;
  Smart::StatusCode disconnect() override;
  Smart::StatusCode blocking(const bool blocking) override;
  Smart::StatusCode subscribe(const int &prescale) override;
  ~PushClientOpcUa() override;
  Smart::StatusCode unsubscribe() override;

  Smart::StatusCode getUpdate(DataType &d) override;

  Smart::StatusCode getUpdateWait(DataType &d,
                                  const std::chrono::steady_clock::duration &timeout = std::chrono::steady_clock::duration::zero()) override;

 private:
  DataType readValueFromSubscription(typename SeRoNet::OPCUA::Client::AsyncSubscriptionOpcUa<DataType>::counter_t offset);
  SeRoNet::OPCUA::Client::UaClientWithMutex_t::shpType m_pUaClientWithMutex;
  std::shared_ptr<SeRoNet::OPCUA::Client::AsyncSubscriptionWithCommObject<DataType>> m_pSubscription;
  std::shared_ptr<SeRoNet::OPCUA::Client::AsyncSubscriptionReader<DataType>> m_pReader;
  open62541::UA_NodeId m_startNodeId;
  SeRoNet::OPCUA::Client::NamingServiceOpcUa m_namingService;
};

template<class DataType>
PushClientOpcUa<DataType>::PushClientOpcUa(
    Smart::IComponent *pComponent,
    open62541::UA_NodeId startNodeId):
    m_pUaClientWithMutex(nullptr),
    m_pSubscription(nullptr),
    Smart::IPushClientPattern<DataType>::IPushClientPattern(pComponent),
    m_startNodeId(startNodeId) {
}

template<class DataType>
Smart::StatusCode PushClientOpcUa<DataType>::subscribe(const int &prescale) {

  m_pSubscription.reset(new SeRoNet::OPCUA::Client::AsyncSubscriptionWithCommObject<DataType>(m_pUaClientWithMutex));
  m_pReader.reset(new SeRoNet::OPCUA::Client::AsyncSubscriptionReader<DataType>(
      std::static_pointer_cast<SeRoNet::OPCUA::Client::AsyncSubscription<DataType>>(m_pSubscription)
                  )
  );

  m_pSubscription->subscribe(m_startNodeId);
  return Smart::StatusCode::SMART_OK;
}

template<class DataType>
Smart::StatusCode PushClientOpcUa<DataType>::unsubscribe() {
  m_pReader = nullptr;
  m_pSubscription = nullptr;
  return Smart::StatusCode::SMART_OK;
}

template<class DataType>
Smart::StatusCode PushClientOpcUa<DataType>::getUpdate(DataType &d) {
  if (m_pReader == nullptr || m_pSubscription == nullptr) {
    return Smart::StatusCode::SMART_UNSUBSCRIBED;
  }

  if (m_pSubscription->empty()) {
    return Smart::StatusCode::SMART_NODATA;
  }

  d = readValueFromSubscription(-1);
  return Smart::StatusCode::SMART_OK;
}

template<class DataType>
Smart::StatusCode PushClientOpcUa<DataType>::getUpdateWait(DataType &d,
                                                           const std::chrono::steady_clock::duration &timeout) {
  if (m_pReader == nullptr || m_pSubscription == nullptr) {
    return Smart::StatusCode::SMART_UNSUBSCRIBED;
  }
  d = readValueFromSubscription(0);
  return Smart::StatusCode::SMART_OK;
}

template<class DataType>
DataType PushClientOpcUa<DataType>::readValueFromSubscription(
    typename SeRoNet::OPCUA::Client::AsyncSubscriptionOpcUa<DataType>::counter_t offset) {
  bool overflow = false;
  typename decltype(m_pSubscription)::element_type::counter_t
  dataRead;
  return m_pSubscription->getData(m_pSubscription->getDataCounter() + offset, overflow, dataRead);
}

template<class DataType>
Smart::StatusCode PushClientOpcUa<DataType>::connect(const std::string &server, const std::string &service) {
  auto retValue = this->m_namingService.getConnectionAndNodeIdByName(server,service);
  m_pUaClientWithMutex = retValue.connection;
  return Smart::StatusCode::SMART_OK;
}

template<class DataType>
Smart::StatusCode PushClientOpcUa<DataType>::disconnect() {
  ///\todo implement
  return Smart::StatusCode::SMART_OK;
}

template<class DataType>
Smart::StatusCode PushClientOpcUa<DataType>::blocking(const bool blocking) {
  ///\todo implement
  return Smart::StatusCode::SMART_OK;
}
template<class DataType>
PushClientOpcUa<DataType>::~PushClientOpcUa() {
  this->unsubscribe();
}

} // namespace Client
} // namespace OPCUA
} // namespace SeRoNet