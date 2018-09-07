///
/// \file EventClient.hpp
/// \author Christian von Arnim
/// \date 06.09.2018
///

#include <SmartSoft_CD_API/smartIEventClientPattern_T.h>
#include <chrono>
#include <memory>
#include "NamingServiceOpcUa.hpp"
#include <iostream>
#include "../Converter/CommObjectToUaArgument.hpp"
#include "../../CommunicationObjects/Description/SelfDescription.hpp"
#include "../Converter/CommObjectToUaVariantArray.hpp"
#include "AsyncSubscriptionWithCommObject.hpp"
#include "AsyncSubscriptionReader.hpp"
#include <atomic>

#pragma once
namespace SeRoNet {
namespace OPCUA {
namespace Client {

template<typename ActivationType, typename EventType>
class EventClient;

namespace Event {
template<typename ActivationType, typename EventType>
struct Id_t {
  ///\FIXME Set back to private
 public:
  friend EventClient<ActivationType, EventType>;
  open62541::UA_NodeId dataNodeId;
  std::shared_ptr<AsyncSubscriptionReader<EventType>> pSubscriptionReader;
  std::atomic_bool invalid {false};
};
}

template<typename ActivationType, typename EventType>
class EventClient : Smart::IEventClientPattern<ActivationType, EventType, std::shared_ptr<Event::Id_t<ActivationType, EventType>>> {
 public:

  typedef std::shared_ptr<Event::Id_t<ActivationType, EventType>> EventIdType;

  EventClient(Smart::IComponent *pComponent);

  Smart::StatusCode connect(const std::string &server, const std::string &service) override;

  Smart::StatusCode disconnect() override;

  Smart::StatusCode blocking(const bool blocking) override;

  Smart::StatusCode activate(const Smart::EventMode &mode, const ActivationType &parameter, EventIdType &id) override;

  Smart::StatusCode deactivate(const EventIdType &id) override;

  Smart::StatusCode tryEvent(const EventIdType &id) override;

  Smart::StatusCode getEvent(const EventIdType &id,
                             EventType &event,
                             const std::chrono::steady_clock::duration &timeout) override;

  Smart::StatusCode getNextEvent(const EventIdType &id,
                                 EventType &event,
                                 const std::chrono::steady_clock::duration &timeout) override;

 private:
  std::string m_server;
  std::string m_service;
  std::shared_ptr<SeRoNet::OPCUA::Client::INamingService> m_namingService;
  SeRoNet::OPCUA::Client::UaClientWithMutex_t::shpType m_pUaClientWithMutex;
  open62541::UA_NodeId m_nodeId;
};

template<typename ActivationType, typename EventType>
Smart::StatusCode EventClient<ActivationType, EventType>::connect(
    const std::string &server,
    const std::string &service) {

  auto retValue = this->m_namingService->getConnectionAndNodeIdByName(server, service);
  m_pUaClientWithMutex = retValue.connection;
  m_nodeId = retValue.nodeId;
  if (!m_pUaClientWithMutex || !m_pUaClientWithMutex->pClient) {
    std::cout << "No valid m_pUaClientWithMutex. " << __FILE__ << ":" << __LINE__ << std::endl;
  }
  return m_pUaClientWithMutex->pClient != nullptr ? Smart::SMART_OK : Smart::SMART_ERROR;
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
    EventIdType &id) {
  /// \FIXME get NodeId from m_nodeId
  //m_nodeId.NodeId->namespaceIndex
  open62541::UA_NodeId methodNodeId(2, "Activation method");

  ActivationType paramCopy(parameter);
  auto selfDesc = CommunicationObjects::Description::SelfDescription(&paramCopy, "");
  auto inArgs = Converter::CommObjectToUaVariantArray(selfDesc.get()).getValue();

  std::size_t outArgSize = 0;
  UA_Variant *pOutArgs = nullptr;
  UA_StatusCode ret = UA_STATUSCODE_GOOD;
  std::cout << "Call with obj: " << m_nodeId << "; method: " << methodNodeId << std::endl;

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
    std::cout << "UA_Client_call failed with: " << UA_StatusCode_name(ret) << std::endl;
    return Smart::SMART_ERROR;
  }

  open62541::UA_ArrayOfVariant outArguments(pOutArgs, outArgSize, true);

  if (outArguments.VariantsSize != 1) {
    std::cout << "Wrong number of reutn values, expect 1, got " << outArguments.VariantsSize << std::endl;
    return Smart::SMART_ERROR;
  }

  auto variantNodeid = outArguments[0];

  if (!variantNodeid.is_a(&UA_TYPES[UA_TYPES_NODEID])) {
    std::cout << "Result is not a NodeId." << std::endl;
    return Smart::SMART_ERROR;
  }

  auto dataNodeId = open62541::UA_NodeId(variantNodeid.getDataAs<UA_NodeId>());

  /// Remove logging
  std::cout << "Got new object of eventdata: " << dataNodeId << std::endl;

  auto asyncSubscription = std::make_shared<AsyncSubscriptionWithCommObject<EventType>>(m_pUaClientWithMutex);


  ret = asyncSubscription->subscribe(dataNodeId);
  if(ret != UA_STATUSCODE_GOOD)
  {
    std::cout << "asyncSubscription->subscribe failed with: " << UA_StatusCode_name(ret) << std::endl;
    return Smart::SMART_ERROR;
  }

  auto subscriptionReader = std::make_shared<AsyncSubscriptionReader<EventType>>(
      std::static_pointer_cast<AsyncSubscription<EventType>>(asyncSubscription)
      );

  id = std::make_shared<typename EventIdType::element_type>();
  id->dataNodeId = dataNodeId;
  id->pSubscriptionReader = subscriptionReader;

  return Smart::SMART_OK;
}

template<typename ActivationType, typename EventType>
Smart::StatusCode EventClient<ActivationType, EventType>::deactivate(const EventIdType &id) {
  id->invalid = true;
  return Smart::SMART_OK;
}

template<typename ActivationType, typename EventType>
Smart::StatusCode EventClient<ActivationType, EventType>::tryEvent(const EventIdType &id) {
  if(!id || id->invalid)
  {
    return Smart::SMART_NOTACTIVATED;
  }
  return id->pSubscriptionReader->hasData()? Smart::SMART_OK : Smart::SMART_ERROR;
}

template<typename ActivationType, typename EventType>
Smart::StatusCode EventClient<ActivationType, EventType>::getEvent(
    const EventIdType &id,
    EventType &event,
    const std::chrono::steady_clock::duration &timeout) {
  if(!id || id->invalid)
  {
    return Smart::SMART_NOTACTIVATED;
  }
  /// TODO respect timeout
  event = id->pSubscriptionReader->getData();
  return Smart::StatusCode::SMART_OK;
}

template<typename ActivationType, typename EventType>
Smart::StatusCode EventClient<ActivationType, EventType>::getNextEvent(
    const EventIdType &id,
    EventType &event,
    const std::chrono::steady_clock::duration &timeout) {
  if(!id || id->invalid)
  {
    return Smart::SMART_NOTACTIVATED;
  }
  id->pSubscriptionReader->seek(0, AsyncSubscriptionReader<EventType>::SEEK_REF::REF_END);
  event = id->pSubscriptionReader->getData();
  return Smart::StatusCode::SMART_OK;
}

template<typename ActivationType, typename EventType>
EventClient<ActivationType, EventType>::EventClient(Smart::IComponent *pComponent)
    : Smart::IEventClientPattern<ActivationType, EventType, EventIdType>(pComponent),
      m_namingService(std::make_shared<SeRoNet::OPCUA::Client::NamingServiceOpcUa>()) {

}

}
}
}