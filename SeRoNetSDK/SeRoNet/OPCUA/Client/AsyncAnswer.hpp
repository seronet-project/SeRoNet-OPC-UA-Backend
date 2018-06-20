#pragma once

#include <mutex>
#include <atomic>
#include <condition_variable>
#include "IBlocking.hpp"
#include "../../Exceptions/BlockingDisabledException.hpp"
#include "../../Exceptions/ResultErrorException.hpp"
#include <string>

namespace SeRoNet {
namespace OPCUA {
namespace Client {

template<typename T_RETURN>
class AsyncAnswer : IBlocking {
 public:
  AsyncAnswer(instanceStorage_t *instStorage, bool blockingEnabled);

 public:
  ~AsyncAnswer() override;

  bool hasAnswer() { return m_hasAnswer; }
  T_RETURN waitForAnswer();

 protected:
  void blockingChanged() override;

  virtual T_RETURN getAnswer() = 0;
  void setHasAnswer(bool hasAnswer = true) {
    m_hasAnswer = hasAnswer;
    m_cv_hasAnswer.notify_all();
  }

  void setError(std::string error_msg)
  {
    m_error_msg = error_msg;
    this->setHasAnswer();
  }
 private:
  std::atomic_bool m_hasAnswer = {false};
  std::string m_error_msg;
  std::condition_variable m_cv_hasAnswer;
};

template<typename T_RETURN>
inline AsyncAnswer<T_RETURN>::AsyncAnswer(instanceStorage_t *instStorage, bool blockingEnabled):
    IBlocking(instStorage, blockingEnabled) {
}

template<typename T_RETURN>
inline AsyncAnswer<T_RETURN>::~AsyncAnswer() {
}

template<typename T_RETURN>
inline T_RETURN AsyncAnswer<T_RETURN>::waitForAnswer() {
  std::mutex cv_m;
  std::unique_lock<decltype(cv_m)> cv_ml(cv_m);
  m_cv_hasAnswer.wait(cv_ml, [&]() -> bool { return this->m_hasAnswer || !BlockingEnabled; });

  if (!this->hasAnswer()) {
    throw SeRoNet::Exceptions::BlockingDisabledException("No AsyncAnswer available.");
  }

  if(!m_error_msg.empty())
  {
    throw SeRoNet::Exceptions::ResultErrorException(m_error_msg);
  }

  return getAnswer();
}

template<typename T_RETURN>
void AsyncAnswer<T_RETURN>::blockingChanged() {
  m_cv_hasAnswer.notify_all();
}

}//  namespace Client
}//  namespace OPCUA
}//  namespace SeRoNet

