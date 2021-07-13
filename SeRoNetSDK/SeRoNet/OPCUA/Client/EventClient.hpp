/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/


#include <smartIEventClientPattern_T.h>
#include <chrono>
#include <memory>
#include "NamingServiceOpcUa.hpp"
#include <iostream>
#include "../Converter/CommObjectToUaArgument.hpp"
#include "../../CommunicationObjects/Description/SelfDescription.hpp"
#include "../Converter/CommObjectToUaVariantArray.hpp"
#include "AsyncSubscriptionWithCommObject.hpp"
#include "AsyncSubscriptionReader.hpp"
#include "../../Utils/Component.hpp"
#include <atomic>
#include <open62541/client_highlevel.h>
#include <smartICorrelationId.h>

#pragma once
namespace SeRoNet {
namespace OPCUA {
namespace Client {

template<typename ActivationType, typename EventType>
class EventClient;

namespace Event {
template<typename ActivationType, typename EventType>
class Id_t : public Smart::ICorrelationId {
  ///\FIXME Set back to private
 public:
  friend EventClient<ActivationType, EventType>;
  open62541Cpp::UA_NodeId dataNodeId;
  std::shared_ptr<AsyncSubscriptionReader<EventType>> pSubscriptionReader;
  std::atomic_bool invalid {false};
  Id_t(const open62541Cpp::UA_NodeId &dataNodeId, const std::shared_ptr<AsyncSubscriptionReader<EventType>> reader)
  : dataNodeId(dataNodeId), pSubscriptionReader(reader) {  }
protected:
  virtual std::string to_string() const override final {
    std::stringstream sstream;
    sstream << pSubscriptionReader;
    return sstream.str();
  }
  virtual bool less_than(const ICorrelationId *other) const override final {
    auto native_other = dynamic_cast<const Id_t<ActivationType,EventType>*>(other);
    //TODO: should we rather compare the dataNodeId here???
    return pSubscriptionReader < native_other->pSubscriptionReader;
  }
  virtual bool equals_to(const ICorrelationId *other) const override final {
    auto native_other = dynamic_cast<const Id_t<ActivationType,EventType>*>(other);
    //TODO: should we rather compare the dataNodeId here???
    return pSubscriptionReader == native_other->pSubscriptionReader;
  }
};
}

template<typename ActivationType, typename EventType>
class EventClient : public Smart::IEventClientPattern<ActivationType, EventType> {
 public:
  EventClient(Utils::Component *pComponent);

  Smart::StatusCode connect(const std::string &server, const std::string &service) override;

  Smart::StatusCode disconnect() override;

  Smart::StatusCode blocking(const bool blocking) override;

  Smart::StatusCode activate(const Smart::EventMode &mode, const ActivationType &parameter, Smart::EventIdPtr &id) override;

  Smart::StatusCode deactivate(const Smart::EventIdPtr id) override;

  Smart::StatusCode tryEvent(const Smart::EventIdPtr id) override;

  Smart::StatusCode getEvent(const Smart::EventIdPtr id,
                             EventType &event,
                             const Smart::Duration &timeout = Smart::Duration::max()) override;

  Smart::StatusCode getNextEvent(const Smart::EventIdPtr id,
                                 EventType &event,
                                 const Smart::Duration &timeout = Smart::Duration::max()) override;

 private:
  std::string m_server;
  std::string m_service;
  std::shared_ptr<SeRoNet::OPCUA::Client::INamingService> m_namingService;
  SeRoNet::OPCUA::Client::UaClientWithMutex_t::shpType m_pUaClientWithMutex;
  open62541Cpp::UA_NodeId m_nodeId;
  bool serviceIsAvailable();
};

template<typename ActivationType, typename EventType>
Smart::StatusCode EventClient<ActivationType, EventType>::connect(
    const std::string &server,
    const std::string &service) {

  auto retValue = this->m_namingService->getConnectionAndNodeIdByName(server, service);
  m_pUaClientWithMutex = retValue.connection;
  m_nodeId = retValue.nodeId;
  if (!m_pUaClientWithMutex || !m_pUaClientWithMutex->pClient) {
    std::cout <<"No valid m_pUaClientWithMutex. " <<__FILE__ <<":" <<__LINE__ <<std::endl;
  }
  if(!m_pUaClientWithMutex->pClient)
  {
    return Smart::SMART_ERROR;
  }
  if(!this->serviceIsAvailable())
  {
    return Smart::SMART_SERVICEUNAVAILABLE;
  }
  return Smart::SMART_OK;
}

template<typename ActivationType, typename EventType>
Smart::StatusCode EventClient<ActivationType, EventType>::disconnect() {

  m_pUaClientWithMutex = nullptr;
  return Smart::SMART_OK;
}

template<typename ActivationType, typename EventType>
Smart::StatusCode EventClient<ActivationType, EventType>::blocking(const bool blocking) {
  return Smart::SMART_ERROR;
}

template<typename ActivationType, typename EventType>
Smart::StatusCode EventClient<ActivationType, EventType>::activate(
    const Smart::EventMode &mode,
    const ActivationType &parameter,
    Smart::EventIdPtr &id) {
  /// \FIXME get NodeId from m_nodeId
  //m_nodeId.NodeId->namespaceIndex
  open62541Cpp::UA_NodeId methodNodeId(2, "Activation method");

  ActivationType paramCopy(parameter);
  auto selfDesc = CommunicationObjects::Description::SelfDescription(&paramCopy, "");
  auto inArgs = Converter::CommObjectToUaVariantArray(selfDesc.get()).getValue();

  std::size_t outArgSize = 0;
  UA_Variant *pOutArgs = nullptr;
  UA_StatusCode ret = UA_STATUSCODE_GOOD;
  std::cout <<"Call with obj: " <<m_nodeId <<"; method: " <<methodNodeId <<std::endl;

  {
    std::unique_lock<decltype(m_pUaClientWithMutex->opcuaMutex)> ul(m_pUaClientWithMutex->opcuaMutex);

    ret = UA_Client_call(
        m_pUaClientWithMutex->pClient.get(),
        *(m_nodeId.NodeId),
        *(methodNodeId.NodeId),
        inArgs.VariantsSize,
        inArgs.Variants,
        &outArgSize,
        &pOutArgs);
  }
  if (ret != UA_STATUSCODE_GOOD) {
    std::cout <<"UA_Client_call failed with: " <<UA_StatusCode_name(ret) <<std::endl;
    return Smart::SMART_ERROR;
  }

  open62541Cpp::UA_ArrayOfVariant outArguments(pOutArgs, outArgSize, true);

  if (outArguments.VariantsSize != 1) {
    std::cout <<"Wrong number of reutn values, expect 1, got " <<outArguments.VariantsSize <<std::endl;
    return Smart::SMART_ERROR;
  }

  auto variantNodeid = outArguments[0];

  if (!variantNodeid.is_a(&UA_TYPES[UA_TYPES_NODEID])) {
    std::cout <<"Result is not a NodeId." <<std::endl;
    return Smart::SMART_ERROR;
  }

  auto dataNodeId = open62541Cpp::UA_NodeId(variantNodeid.getDataAs<UA_NodeId>());

  /// Remove logging
  std::cout <<"Got new object of eventdata: " <<dataNodeId <<std::endl;

  auto asyncSubscription = std::make_shared<AsyncSubscriptionWithCommObject<EventType>>(m_pUaClientWithMutex);


  ret = asyncSubscription->subscribe(dataNodeId);
  if(ret != UA_STATUSCODE_GOOD)
  {
    std::cout <<"asyncSubscription->subscribe failed with: " <<UA_StatusCode_name(ret) <<std::endl;
    return Smart::SMART_ERROR;
  }

  auto subscriptionReader = std::make_shared<AsyncSubscriptionReader<EventType>>(
      std::static_pointer_cast<AsyncSubscription<EventType>>(asyncSubscription)
      );

  id = std::make_shared<Event::Id_t<ActivationType,EventType>>(dataNodeId, subscriptionReader);

  return Smart::SMART_OK;
}

template<typename ActivationType, typename EventType>
Smart::StatusCode EventClient<ActivationType, EventType>::deactivate(const Smart::EventIdPtr id) {
  auto internal_id = std::dynamic_pointer_cast<Event::Id_t<ActivationType,EventType>>(id);
  if(internal_id) {
    internal_id->invalid = true;
  }
  return Smart::SMART_OK;
}

template<typename ActivationType, typename EventType>
Smart::StatusCode EventClient<ActivationType, EventType>::tryEvent(const Smart::EventIdPtr id) {
  auto internal_id = std::dynamic_pointer_cast<Event::Id_t<ActivationType,EventType>>(id);
  if(!internal_id )
  {
    return Smart::SMART_WRONGID;
  }else if(internal_id->invalid)
  {
    return Smart::SMART_NOTACTIVATED;
  }
  return internal_id->pSubscriptionReader->hasData()? Smart::SMART_OK : Smart::SMART_ACTIVE;
}

template<typename ActivationType, typename EventType>
bool EventClient<ActivationType, EventType>::serviceIsAvailable()
{
    if(!m_pUaClientWithMutex)
    {
      return false;
    }

    UA_NodeClass tmpOut;
    UA_StatusCode status;
    {
      std::unique_lock<decltype(m_pUaClientWithMutex->opcuaMutex)> lock(m_pUaClientWithMutex->opcuaMutex);
      status = UA_Client_readNodeClassAttribute(m_pUaClientWithMutex->pClient.get(), *this->m_nodeId.NodeId, &tmpOut);
    }
    if( status != UA_STATUSCODE_GOOD)
    {
      m_pUaClientWithMutex.reset();
      return false;
    }
    return true;
  }
template<typename ActivationType, typename EventType>
Smart::StatusCode EventClient<ActivationType, EventType>::getEvent(
    const Smart::EventIdPtr id,
    EventType &event,
    const Smart::Duration &timeout) {
  auto internal_id = std::dynamic_pointer_cast<Event::Id_t<ActivationType,EventType>>(id);
  if(!this->serviceIsAvailable())
  {
    return Smart::SMART_DISCONNECTED;
  }
  if(!internal_id )
  {
    return Smart::SMART_WRONGID;
  }
  if(internal_id->invalid)
  {
    return Smart::SMART_NOTACTIVATED;
  }
  /// TODO respect timeout
  event = internal_id->pSubscriptionReader->getData();
  return Smart::StatusCode::SMART_OK;
}

template<typename ActivationType, typename EventType>
Smart::StatusCode EventClient<ActivationType, EventType>::getNextEvent(
    const Smart::EventIdPtr id,
    EventType &event,
    const Smart::Duration &timeout) {
  auto internal_id = std::dynamic_pointer_cast<Event::Id_t<ActivationType,EventType>>(id);
  if(!internal_id )
  {
    return Smart::SMART_WRONGID;
  }else if(internal_id->invalid)
  {
    return Smart::SMART_NOTACTIVATED;
  }
  internal_id->pSubscriptionReader->seek(0, AsyncSubscriptionReader<EventType>::SEEK_REF::REF_END);
  event = internal_id->pSubscriptionReader->getData();
  return Smart::StatusCode::SMART_OK;
}

template<typename ActivationType, typename EventType>
EventClient<ActivationType, EventType>::EventClient(Utils::Component *pComponent)
    : Smart::IEventClientPattern<ActivationType, EventType>(pComponent),
      m_namingService(std::make_shared<SeRoNet::OPCUA::Client::NamingServiceOpcUa>(pComponent->getOpcUaServer())) {

}

}
}
}
