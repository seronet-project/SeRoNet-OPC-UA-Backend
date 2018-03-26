#ifndef ASYNC_SUBSCRIPTION_HPP
#define ASYNC_SUBSCRIPTION_HPP

#include <atomic>
#include <condition_variable>
#include <mutex>

namespace SeRoNet {
namespace OPCUA {
namespace Client {

///TODO Add invalid flag
/// T_DATATYPE must be assignable (operator=) and constructable
template<typename T_DATATYPE>
///TODO Add invalid flag
class AsyncSubscription {
 public:
  typedef std::atomic<std::int64_t> atomic_counter_t;
  typedef std::int64_t counter_t;

  virtual ~AsyncSubscription() = 0;

  //Functions for Acquire data, easy Access with AsyncSubscriptionReader

  ///@todo Implement Iterator instead of reader (Iterator Interface, ++, *, begin(), end()...) !?

  /// Get the DataCounter after the last element (similar to end())
  virtual counter_t getDataCounter() = 0;
  virtual bool hasData(counter_t dataCounter) = 0;
  virtual T_DATATYPE getData(counter_t dataCounter, bool &overflow, counter_t &readDataCounter) = 0;
  virtual bool empty() = 0;
 protected:
  virtual void addData(const T_DATATYPE &newData) = 0;
};

template<typename T_DATATYPE>
AsyncSubscription<T_DATATYPE>::~AsyncSubscription() {
}
}
}
}

#endif
