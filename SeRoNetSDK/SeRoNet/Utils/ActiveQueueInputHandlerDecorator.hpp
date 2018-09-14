//
// Created by Sebastian Friedl on 14.02.2018.
//

#ifndef SERONETSDK_ACTIVEQUEUEINPUTHANDLERDECORATOR_HPP
#define SERONETSDK_ACTIVEQUEUEINPUTHANDLERDECORATOR_HPP

#include "smartIActiveQueueInputHandlerDecorator_T.h"
#include "Task.hpp"
#include "../OPCUA/Server/QueryServerHandler.hpp"

namespace SeRoNet {
namespace Utils {
template<class InputType>
class ActiveQueueInputHandlerDecoratorImpl
    : public Smart::IActiveQueueInputHandlerDecorator<InputType> {
 private:
  std::thread m_thread;

  std::atomic_bool m_interruption_requested = {false};

 protected:
  void sleep_for(const std::chrono::steady_clock::duration &rel_time) override {
    throw SeRoNet::Exceptions::NotImplementedException(__FUNCTION__);
  }

 public:
  ActiveQueueInputHandlerDecoratorImpl(Smart::IComponent *component,
                                       Smart::IInputHandler<InputType> *inner_handler)
      : Smart::IActiveQueueInputHandlerDecorator<InputType>(component,
                                                            inner_handler) {}

  virtual ~ActiveQueueInputHandlerDecoratorImpl() {
    this->stop();
  }

  /** Creates and starts a new thread (if not yet started)
 */
  int start() override {
    if (!m_thread.joinable()) {
      m_thread = std::thread(&Task::task_execution, this);
    }
    return 0;
  }

  /** Closes currently active thread (if it was started before)
   */
  int stop(const bool wait_till_stopped = true) override {
    if (m_thread.joinable()) {
      this->m_interruption_requested.store(true);
      m_thread.join();
    }
    return 0;
  }

  /** Tests whether the thread has been signaled to stop.
 *
 * This method allows to implement cooperative thread stopping.
 */
  bool test_canceled() override {
    return this->m_interruption_requested.load();
  }

};

template<class RequestType, class AnswerType>
class ActiveQueueQueryServerHandlerDecoratorImpl
    : public Smart::IActiveQueueQueryServerHandlerDecorator<RequestType, AnswerType, int>
  //TODO int durch smartID ersetzten
{
 private:
  std::thread m_thread;

  std::atomic_bool m_interruption_requested = {false};

 protected:
  void sleep_for(const std::chrono::steady_clock::duration &rel_time) override {
    throw SeRoNet::Exceptions::NotImplementedException(__FUNCTION__);
  }

 public:
  ActiveQueueQueryServerHandlerDecoratorImpl(Smart::IComponent *component,
                                             OPCUA::Server::QueryServerHandler<RequestType, AnswerType>
                                             *inner_handler)
      : Smart::IActiveQueueQueryServerHandlerDecorator<RequestType, AnswerType, int>
            (component,
             inner_handler)        //TODO int durch smartID ersetzten

  {}

  virtual ~ActiveQueueQueryServerHandlerDecoratorImpl() {
    this->stop();
  }

  /** Creates and starts a new thread (if not yet started)
 */
  int start() override {
    if (!m_thread.joinable()) {
      m_thread = std::thread(&Task::task_execution, this);
    }
    return 0;
  }

  /** Closes currently active thread (if it was started before)
   */
  int stop(const bool wait_till_stopped = true) override {
    if (m_thread.joinable()) {
      this->cancel_processing();
      this->m_interruption_requested.store(true);
      m_thread.join();
    }
    return 0;
  }

  /** Tests whether the thread has been signaled to stop.
 *
 * This method allows to implement cooperative thread stopping.
 */
  bool test_canceled() override {
    return this->m_interruption_requested.load();
  }

};
}
}

#endif //SERONETSDK_ACTIVEQUEUEINPUTHANDLERDECORATOR_HPP
