#ifndef ASYNC_SUBSCRIPTION_ARRAY_BUFFER_HPP
#define ASYNC_SUBSCRIPTION_ARRAY_BUFFER_HPP

#include "AsyncSubscription.hpp"
#include <memory>
#include <mutex>
#include <shared_mutex>

namespace SeRoNet {
namespace OPCUA {
namespace Client {

/// T_DATATYPE must be assignable (operator=) and constructable
template<typename T_DATATYPE>
class AsyncSubscriptionArrayBuffer : public AsyncSubscription<T_DATATYPE> {
 public:
  explicit AsyncSubscriptionArrayBuffer(std::size_t queueSize = 8)
      : m_bufferSize(queueSize), m_dataBuffer(new T_DATATYPE[queueSize]) {
  }

  virtual ~AsyncSubscriptionArrayBuffer() = 0;

  bool empty() override;

  /// Return counter, pointing to the element after end()
  typename AsyncSubscription<T_DATATYPE>::counter_t getDataCounter() override { return m_dataCounter; }

  bool hasData(typename AsyncSubscription<T_DATATYPE>::counter_t dataCounter) { return m_dataCounter > dataCounter; }

  ///TODO? Use default arguments (static variables) for overflow and readDatacounter?
  T_DATATYPE getData(
      typename AsyncSubscription<T_DATATYPE>::counter_t dataCounter,
      bool &overflow,
      typename AsyncSubscription<T_DATATYPE>::counter_t &readDataCounter) override;

 protected:
  void addData(const T_DATATYPE &newData) override {
    std::unique_lock<decltype(m_dataBufferLock)> lock(m_dataBufferLock);
    std::size_t nextIndex = static_cast<std::size_t> (m_dataCounter % m_bufferSize);
    m_dataBuffer[nextIndex] = newData;
    ++m_dataCounter;
    m_cv_hasData.notify_all();
  }

 private:
  T_DATATYPE *m_dataBuffer;
  std::shared_mutex m_dataBufferLock;
  const std::size_t m_bufferSize;

  typename AsyncSubscription<T_DATATYPE>::atomic_counter_t m_dataCounter = 0;
  std::condition_variable m_cv_hasData;
};

template<typename T_DATATYPE>
AsyncSubscriptionArrayBuffer<T_DATATYPE>::~AsyncSubscriptionArrayBuffer() {
}

template<typename T_DATATYPE>
bool AsyncSubscriptionArrayBuffer<T_DATATYPE>::empty() {
  return m_dataCounter == 0;
}

template<typename T_DATATYPE>
T_DATATYPE AsyncSubscriptionArrayBuffer<T_DATATYPE>::getData(typename AsyncSubscription<T_DATATYPE>::counter_t dataCounter,
                                                             bool &overflow,
                                                             typename AsyncSubscription<T_DATATYPE>::counter_t &readDataCounter) {
  ///TODO Assert dataCounter  <= m_dataCounter (max next unread value?!)

  std::shared_lock<decltype(m_dataBufferLock)> lock(m_dataBufferLock);

  // Wait for data if no data is availiable
  if (!this->hasData(dataCounter)) {
    lock.unlock(); //Unlock so new values are possible
    std::mutex cv_m;
    std::unique_lock<decltype(cv_m)> cv_ml(cv_m);
    m_cv_hasData.wait(cv_ml, [&]() -> bool { return this->hasData(dataCounter); });
    lock.lock();
  }

  ///@todo assert datacounter < m_dataCounter
  overflow = (m_dataCounter - dataCounter) > m_bufferSize;

  // Check if buffer has overflow (values has not been read and has been discarded)
  if (overflow) {
    //Read oldest/nearest valid value
    readDataCounter = m_dataCounter - m_bufferSize;
  } else {
    readDataCounter = dataCounter;
  }
  std::size_t index = static_cast<std::size_t> (readDataCounter % m_bufferSize);
  return m_dataBuffer[index];
}
}
}
}

#endif
