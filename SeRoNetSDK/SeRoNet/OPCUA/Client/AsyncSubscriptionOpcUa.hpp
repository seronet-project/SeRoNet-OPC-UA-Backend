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

  static void handler_ValueChanged(UA_UInt32 monId, UA_DataValue *value, void *context);

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
  UA_StatusCode ret;
  ///TODO set interval
  auto subscSettings = UA_SubscriptionSettings_default;
  subscSettings.requestedPublishingInterval = 250;
  subscSettings.requestedLifetimeCount = 2500;
  ret = UA_Client_Subscriptions_new(m_pUaClientWithMutex->pClient.get(), UA_SubscriptionSettings_default, &m_subId);
  if (ret != UA_STATUSCODE_GOOD) {
    std::cout << "Nongood status code: " << ret << std::endl;
    return ret;
  }

  ///\todo exception
  assert(m_monItems.empty());
  assert(m_monitoredItemsIdorder.empty());

  //UA_NodeId monitorThis = UA_NODEID_STRING(1, "the.answer");
  UA_UInt32 monId = 0;
  for (auto &nodeid : nodeIds) {

    ret = UA_Client_Subscriptions_addMonitoredItem(m_pUaClientWithMutex->pClient.get(),
                                                   m_subId,
                                                   nodeid,
                                                   UA_ATTRIBUTEID_VALUE,
                                                   &handler_ValueChanged,
                                                   this,
                                                   &monId,
                                                   250);
    if (ret != UA_STATUSCODE_GOOD) {
      /// TODO Logging
      std::cout << "Nongood status code while adding MonitoredItem: " << ret << std::endl;
      unsubscribe();
      return ret;
    }
    m_monitoredItemsIdorder.push_back(monId);

    monItemInfo_t info;
    info.nodeId = open62541::UA_NodeId(nodeid);
    m_monItems.insert(typename monItems_t::value_type(monId, info));
  }

  /// Send first request
  UA_Client_Subscriptions_manuallySendPublishRequest_Async(m_pUaClientWithMutex->pClient.get());
  return ret;
}

template<typename T_DATATYPE>
inline void AsyncSubscriptionOpcUa<T_DATATYPE>::unsubscribe() {
  std::unique_lock<decltype(m_pUaClientWithMutex->opcuaMutex)> lock(m_pUaClientWithMutex->opcuaMutex);
  /// Check if there is a valid subscription
  if (m_subId != 0) {
    UA_StatusCode ret;
    ret = UA_Client_Subscriptions_remove(m_pUaClientWithMutex->pClient.get(), m_subId);
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
inline void AsyncSubscriptionOpcUa<T_DATATYPE>::handler_ValueChanged(UA_UInt32 monId,
                                                                     UA_DataValue *value,
                                                                     void *context) {
  AsyncSubscriptionOpcUa<T_DATATYPE> *ptr = static_cast<AsyncSubscriptionOpcUa<T_DATATYPE> *>(context);
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
