/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#pragma once

#include "AsyncSubscriptionOpcUa.hpp"
#include "AsyncSubscriptionReader.hpp"
#include <SmartSoft_CD_API/smartIPushClientPattern_T.h>
#include "AsyncSubscriptionWithCommObject.hpp"
#include "../../Utils/Component.hpp"

#include <open62541/client_highlevel_async.h>
#include <memory>
#include <iostream>
#include "UaClientWithMutex.hpp"
#include "NamingServiceOpcUa.hpp"

namespace SeRoNet {
namespace OPCUA {
namespace Client {

template<class DataType>
class PushClient :
    public Smart::IPushClientPattern<DataType> {
 public:
  explicit PushClient(Utils::Component *pComponent);
  Smart::StatusCode connect(const std::string &server, const std::string &service) override;
  Smart::StatusCode disconnect() override;
  Smart::StatusCode blocking(const bool blocking) override;
  Smart::StatusCode subscribe(const int &prescale) override;
  ~PushClient() override;
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
  std::shared_ptr<SeRoNet::OPCUA::Client::NamingServiceOpcUa> m_namingService;
};

template<class DataType>
PushClient<DataType>::PushClient(
    Utils::Component *pComponent):
    m_pUaClientWithMutex(nullptr),
    m_pSubscription(nullptr),
    m_namingService(std::make_shared<SeRoNet::OPCUA::Client::NamingServiceOpcUa>(pComponent->getOpcUaServer())),
    Smart::IPushClientPattern<DataType>::IPushClientPattern(pComponent) {
}

template<class DataType>
Smart::StatusCode PushClient<DataType>::subscribe(const int &prescale) {

  if (m_pSubscription) {
    m_pSubscription->setOnDataCallback(); // Reset callback
  }
  m_pSubscription.reset(new SeRoNet::OPCUA::Client::AsyncSubscriptionWithCommObject<DataType>(m_pUaClientWithMutex));
  m_pReader.reset(new SeRoNet::OPCUA::Client::AsyncSubscriptionReader<DataType>(
      std::static_pointer_cast<SeRoNet::OPCUA::Client::AsyncSubscription<DataType>>(m_pSubscription)
                  )
  );

  m_pSubscription->setOnDataCallback(
      [&](const DataType &data) {
        this->notify_input(data);
      }
  );

  m_pSubscription->subscribe(m_startNodeId);
  return Smart::StatusCode::SMART_OK;
}

template<class DataType>
Smart::StatusCode PushClient<DataType>::unsubscribe() {
  m_pReader = nullptr;
  m_pSubscription = nullptr;
  return Smart::StatusCode::SMART_OK;
}

template<class DataType>
Smart::StatusCode PushClient<DataType>::getUpdate(DataType &d) {
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
Smart::StatusCode PushClient<DataType>::getUpdateWait(DataType &d,
                                                      const std::chrono::steady_clock::duration &timeout) {
  if (m_pReader == nullptr || m_pSubscription == nullptr) {
    return Smart::StatusCode::SMART_UNSUBSCRIBED;
  }
  d = readValueFromSubscription(0);
  return Smart::StatusCode::SMART_OK;
}

template<class DataType>
DataType PushClient<DataType>::readValueFromSubscription(
    typename SeRoNet::OPCUA::Client::AsyncSubscriptionOpcUa<DataType>::counter_t offset) {
  bool overflow = false;
  typename decltype(m_pSubscription)::element_type::counter_t dataRead;
  return m_pSubscription->getData(m_pSubscription->getDataCounter() + offset, overflow, dataRead);
}

template<class DataType>
Smart::StatusCode PushClient<DataType>::connect(const std::string &server, const std::string &service) {
  auto retValue = this->m_namingService->getConnectionAndNodeIdByName(server, service);
  m_pUaClientWithMutex = retValue.connection;
  m_startNodeId = retValue.nodeId;
  return Smart::StatusCode::SMART_OK;
}

template<class DataType>
Smart::StatusCode PushClient<DataType>::disconnect() {
  ///\todo unsubscribe
  m_pUaClientWithMutex = nullptr;
  return Smart::StatusCode::SMART_OK;
}

template<class DataType>
Smart::StatusCode PushClient<DataType>::blocking(const bool /*blocking*/) {
  ///\todo implement
  return Smart::StatusCode::SMART_OK;
}
template<class DataType>
PushClient<DataType>::~PushClient() {
  this->unsubscribe();
}

} // namespace Client
} // namespace OPCUA
} // namespace SeRoNet
