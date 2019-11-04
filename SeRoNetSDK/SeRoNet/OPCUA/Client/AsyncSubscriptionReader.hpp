/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/


#ifndef ASYNC_SUBSCRIPTIONREADER_HPP
#define ASYNC_SUBSCRIPTIONREADER_HPP

#include <atomic>
#include <condition_variable>
#include <memory>
#include <iostream>
#include "AsyncSubscription.hpp"
#include "../../Exceptions/NoDataAvailableException.hpp"

namespace SeRoNet {
namespace OPCUA {
namespace Client {

template<typename T_DATATYPE>
class AsyncSubscription;

/// Reading values from an subscription one by one
template<typename T_DATATYPE>
class AsyncSubscriptionReader {
 public:
  ///TODO Asser pSubscription != null
  explicit AsyncSubscriptionReader(std::shared_ptr<AsyncSubscription<T_DATATYPE>> pSubscription) : m_pSubscription(
      pSubscription) {}

  virtual bool isOverflow() final { return m_overflow; }
  virtual void resetOverflow() final { m_overflow = false; }

  enum SEEK_REF {
    REF_FIRST, ///<First available Data in the buffer
    REF_CURRENT, ///<Current position of the reader
    REF_END ///<One after the last value of the buffer
  };

  void seek(std::int64_t offset, SEEK_REF referece = REF_CURRENT);

  ///TODO Check invalid
  bool hasData() {
    return !m_pSubscription->empty() && m_pSubscription->hasData(m_dataFetchCounter);
  }

  ///TODO Check invalid
  /// Blocking Call Get Data and increment read counter
  T_DATATYPE getData();

  /// Set the counter to the element after the last element and return the last read element
  /// Throw an exception if no Data is available
  /// @todo remove!? (could be build from other functions and modify the fetchCounter,
  /// 	which my be unpredicted by the user)
  T_DATATYPE getLastAvailableData();

  ///@return true, when the underlying subscription has no data
  bool empty() { return m_pSubscription->empty(); }

 private:
  std::shared_ptr<AsyncSubscription<T_DATATYPE>> m_pSubscription;
  typename AsyncSubscription<T_DATATYPE>::counter_t m_dataFetchCounter = 0;
  bool m_overflow = false;
};

template<typename T_DATATYPE>
void AsyncSubscriptionReader<T_DATATYPE>::seek(std::int64_t offset, AsyncSubscriptionReader::SEEK_REF referece) {
  typename AsyncSubscription<T_DATATYPE>::counter_t endCounter, newRequestedCounter;
  endCounter = m_pSubscription->getDataCounter() + 1;

  switch (referece) {
    case REF_FIRST: newRequestedCounter = offset;
      break;
    case REF_CURRENT: newRequestedCounter = m_dataFetchCounter + offset;
      break;
    case REF_END: newRequestedCounter = endCounter + offset;
      break;
    default:
      ///@todo error handling
      std::cout <<"Unknown reference in " <<__FUNCTION__ <<std::endl;
      return;
  }

  if (newRequestedCounter <= endCounter) {
    m_dataFetchCounter = newRequestedCounter;
  } else if (newRequestedCounter >= 0) {
    ///@todo logging + exception
    std::cout <<"Warning, request Counter after end in " <<__FUNCTION__ <<std::endl;
  } else {
    ///newRequestedCounter <0
    ///@todo logging + exception
    std::cout <<"Warning, request Counter before begin in " <<__FUNCTION__ <<std::endl;
  }
}

template<typename T_DATATYPE>
T_DATATYPE AsyncSubscriptionReader<T_DATATYPE>::getData() {
  bool overflow;
  typename AsyncSubscription<T_DATATYPE>::counter_t dataReadCounter;
  T_DATATYPE ret = m_pSubscription->getData(m_dataFetchCounter, overflow, dataReadCounter);
  m_dataFetchCounter = dataReadCounter;
  ++m_dataFetchCounter; //Increment to the next element
  m_overflow |= overflow;
  return ret;
}

template<typename T_DATATYPE>
T_DATATYPE AsyncSubscriptionReader<T_DATATYPE>::getLastAvailableData() {
  if (empty()) {
    throw Exceptions::NoDataAvailableException("No Data available in subscription.");
  }
  seek(-1, REF_END);
  return getData();
}
}
}
}

#endif
