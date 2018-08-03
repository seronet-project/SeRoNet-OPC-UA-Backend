#ifndef ASYNC_SUBSCRIPTION_OPCUA_HPP
#define ASYNC_SUBSCRIPTION_OPCUA_HPP

#include "AsyncSubscriptionArrayBuffer.hpp"

#include <memory>
#include <vector>
#include <iostream>
#include <map>
#include <list>
#include <Open62541Cpp/UA_NodeId.hpp>
#include <Open62541Cpp/UA_DataValue.hpp>
#include <open62541.h>
#include <mutex>
#include <open62541/open62541.h>
#include "UaClientWithMutex.hpp"
#include "../../Exceptions/SeRoNetSDKException.hpp"
#include "../../Exceptions/NotImplementedException.hpp"

namespace SeRoNet {
namespace OPCUA {
namespace Client {

template<typename T_DATATYPE>
class AsyncSubscriptionOpcUa : public AsyncSubscriptionArrayBuffer<T_DATATYPE> {
 public:
  ///@todo rework with Client connection pool
  AsyncSubscriptionOpcUa(UaClientWithMutex_t::shpType pUaClientwithMutex);

  ///\todo rework to take iterators
  virtual UA_StatusCode subscribe(std::vector<UA_NodeId> nodeIds);
  void unsubscribe();
  virtual ~AsyncSubscriptionOpcUa();

  bool isSubscribed() { return m_subId != 0; }

  struct monItemInfo_t {
    open62541::UA_NodeId nodeId;
    ///TODO use std::optional instead of std::shared_ptr
    std::shared_ptr<open62541::UA_DataValue> value;
  };

 protected:
  UaClientWithMutex_t::shpType m_pUaClientWithMutex;

  typedef std::list<monItemInfo_t> listOfNodeIdValue_t;
  /// Called with a list of nodeid value pairs, order is the same like the order of the subscribed nodeIds
  /// This function must be overridden in a derived class, the implementation inside this class only ensures, that no
  /// "pure virtual method called" error is generated when this function is called by a different thread during
  /// destruction.
  /// \param nodeIdvalues
  virtual void processValues(listOfNodeIdValue_t listOfNodeIdvalues);

 private:

  /// Called when valid values for each item has been received
  void processValues();

  void valueChanged(UA_UInt32 monId, UA_DataValue *value);

  static void handler_ValueChanged(
      UA_Client *client,
      UA_UInt32 subId,
      void *subContext,
      UA_UInt32 monId, void *monContext,
      UA_DataValue *value
      );

  UA_UInt32 m_subId = 0;

  ///TODO other map type!?
  typedef std::list<UA_UInt32> listOfMonitoredItemIds_t;
  listOfMonitoredItemIds_t m_monitoredItemsIdorder;
  /// Index is monitored ItemID
  typedef std::map<UA_UInt32, monItemInfo_t> monItems_t;
  monItems_t m_monItems;
  std::size_t m_valuesSet = 0;
};

template<typename T_DATATYPE>
inline AsyncSubscriptionOpcUa<T_DATATYPE>::AsyncSubscriptionOpcUa(UaClientWithMutex_t::shpType pUaClientwithMutex)
    : m_pUaClientWithMutex(pUaClientwithMutex) {

}

template<typename T_DATATYPE>
inline UA_StatusCode AsyncSubscriptionOpcUa<T_DATATYPE>::subscribe(std::vector<UA_NodeId> nodeIds) {
  std::unique_lock<decltype(m_pUaClientWithMutex->opcuaMutex)> lock(m_pUaClientWithMutex->opcuaMutex);
  ///TODO assert nodeIDs.size() > 0
  if (isSubscribed()) {
    unsubscribe();
  }
  UA_StatusCode ret = UA_STATUSCODE_GOOD;
  ///TODO set interval
  UA_CreateSubscriptionRequest subscRequest = UA_CreateSubscriptionRequest_default();
  subscRequest.requestedPublishingInterval = 250;
  subscRequest.requestedLifetimeCount = 250;
  /// \todo use callbacks for state updates
  UA_CreateSubscriptionResponse subscResponse = UA_Client_Subscriptions_create(
      m_pUaClientWithMutex->pClient.get(), subscRequest, NULL, NULL, NULL);
  m_subId = subscResponse.subscriptionId;

  if (subscResponse.responseHeader.serviceResult != UA_STATUSCODE_GOOD) {
    std::cout << "Nongood status code: " << ret << std::endl;
    return ret;
  }

  ///\todo exception
  assert(m_monItems.empty());
  assert(m_monitoredItemsIdorder.empty());

  for (auto &nodeId : nodeIds) {
    UA_MonitoredItemCreateRequest monItemRequest =
        UA_MonitoredItemCreateRequest_default(nodeId);
    monItemRequest.monitoringMode = UA_MONITORINGMODE_REPORTING;
    monItemRequest.requestedParameters.queueSize = 3;
    /// \todo set elsewhere
    monItemRequest.requestedParameters.samplingInterval = 250;
    /// \todo test DataChangeFilter with STATUS_VALUE_TIMESTAMP_2
    /*monItemRequest.requestedParameters.filter.encoding = UA_EXTENSIONOBJECT_DECODED;
    auto dataChaneFilter = UA_DataChangeFilter_new();
    UA_DataChangeFilter_init(dataChaneFilter);
    dataChaneFilter->trigger = UA_DataChangeTrigger::UA_DATACHANGETRIGGER_STATUSVALUETIMESTAMP;
    monItemRequest.requestedParameters.filter.content.decoded.data = dataChaneFilter;
    monItemRequest.requestedParameters.filter.content.decoded.type = &(UA_TYPES[UA_TYPES_DATACHANGEFILTER]);
    */
    UA_MonitoredItemCreateResult monItemResponse =
        UA_Client_MonitoredItems_createDataChange(m_pUaClientWithMutex->pClient.get(), m_subId,
                                                  UA_TIMESTAMPSTORETURN_BOTH,
                                                  monItemRequest, this, &handler_ValueChanged, NULL);

    if (monItemResponse.statusCode != UA_STATUSCODE_GOOD) {
      /// TODO Logging
      std::cout << "Nongood status code while adding MonitoredItem: " << ret << std::endl;
      unsubscribe();
      return ret;
    }
    m_monitoredItemsIdorder.push_back(monItemResponse.monitoredItemId);

    monItemInfo_t info;
    info.nodeId = open62541::UA_NodeId(nodeId);
    m_monItems.insert(typename monItems_t::value_type(monItemResponse.monitoredItemId, info));
  }

  return ret;
}

template<typename T_DATATYPE>
inline void AsyncSubscriptionOpcUa<T_DATATYPE>::unsubscribe() {
  std::unique_lock<decltype(m_pUaClientWithMutex->opcuaMutex)> lock(m_pUaClientWithMutex->opcuaMutex);
  /// Check if there is a valid subscription
  if (m_subId != 0) {
    UA_StatusCode ret;
    ret = UA_Client_Subscriptions_deleteSingle(m_pUaClientWithMutex->pClient.get(), m_subId);
    if (ret != UA_STATUSCODE_GOOD) {
      ///TODO Logging
      std::cout << "Nongood status code while unsubscribing:" << ret << std::endl;
    }
    m_subId = 0;
  }

  m_monItems.clear();
  m_valuesSet = 0;
}

template<typename T_DATATYPE>
inline AsyncSubscriptionOpcUa<T_DATATYPE>::~AsyncSubscriptionOpcUa() {
  unsubscribe();
}

template<typename T_DATATYPE>
inline void AsyncSubscriptionOpcUa<T_DATATYPE>::processValues() {
  listOfNodeIdValue_t listOfValues;
  for (const auto &monitoredItemId: m_monitoredItemsIdorder) {
    auto iter = m_monItems.find(monitoredItemId);
    if (iter == m_monItems.end()) {
      //Should not occure
      //\todo define new exception
      throw Exceptions::SeRoNetSDKException("Not all elements found.");
    }

    listOfValues.push_back(iter->second);
    //Reset the value
    iter->second.value.reset();
  }

  m_valuesSet = 0;

  this->processValues(listOfValues);
}

template<typename T_DATATYPE>
inline void AsyncSubscriptionOpcUa<T_DATATYPE>::valueChanged(UA_UInt32 monId, UA_DataValue *value) {
  auto itValue = this->m_monItems.find(monId);

  if (itValue == m_monItems.end()) {
    ///TOOD Logging
    std::cout << "Monitored Item id unknown: " << monId << std::endl;
    return;
  } else {

    if (itValue->second.value == nullptr) {
      ++m_valuesSet;
    } else {
      std::cout << "Override previous value" << std::endl;
    }

    itValue->second.value.reset(new open62541::UA_DataValue(value));

    if (m_valuesSet == m_monItems.size()) {
      processValues();
    }

  }

}

template<typename T_DATATYPE>
inline void AsyncSubscriptionOpcUa<T_DATATYPE>::handler_ValueChanged(
    UA_Client */*client*/,
    UA_UInt32 /*subId*/,
    void */*subContext*/,
    UA_UInt32 monId, void *monContext,
    UA_DataValue *value)
{
  ///\todo use monContext instead of internal list of monIds! (easier code)
  AsyncSubscriptionOpcUa<T_DATATYPE> *ptr = static_cast<AsyncSubscriptionOpcUa<T_DATATYPE> *>(monContext);
  ptr->valueChanged(monId, value);
}

template<typename T_DATATYPE>
inline void AsyncSubscriptionOpcUa<T_DATATYPE>::processValues(AsyncSubscriptionOpcUa::listOfNodeIdValue_t) {
  // This function is intended to be overridden, so no implementation availiable
  std::cout << "Value ignored" << std::endl;
}

}
}
}

#endif
