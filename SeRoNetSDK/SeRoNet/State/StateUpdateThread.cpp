///
/// \file StateUpdateThread.cpp
/// \author Christian von Arnim
/// \date 30.07.2018
///

#include "StateUpdateThread.hpp"
#include <thread>
#include <chrono>
#include "../Exceptions/NotImplementedException.hpp"

#include "StateSlave.hpp"

namespace SeRoNet {
namespace State {

//
//
//
void StateUpdateThread::init(StateSlave *ptr) {
  is_running = true;
  stateServer = ptr;
}

int StateUpdateThread::on_execute() {
  //<alexej date="2010-08-10"/>
  if (is_running) {
    stateServer->updateStateFromThread();
  } else {
    return -1;
  }
  return 0;
}

void StateUpdateThread::halt() {
  is_running = false;
}
int StateUpdateThread::on_entry() {
  return 0;
}
int StateUpdateThread::on_exit() {
  return 0;
}
bool StateUpdateThread::test_canceled() {
  return false;
}
void StateUpdateThread::sleep_for(const std::chrono::steady_clock::duration &rel_time) {
  std::this_thread::sleep_for(rel_time);
}
int StateUpdateThread::start() {
  if (!m_thread.joinable()) {
    m_thread = std::thread(&StateUpdateThread::task_execution, this);
  }
  return 0;
}
int StateUpdateThread::stop(const bool wait_till_stopped) {
  halt();
  cancelTrigger();
  if (m_thread.joinable()) {
    m_interruption_requested = true;
    m_thread.join();
  }
  return 0;
}

}
}
