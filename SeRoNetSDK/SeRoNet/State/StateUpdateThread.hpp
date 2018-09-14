///
/// \file StateUpdateThread.hpp
/// \author Christian von Arnim
/// \date 30.07.2018
///

#pragma once

#include <thread>
#include <atomic>
#include "smartIManagedTask.h"

namespace SeRoNet {

namespace State {
class StateSlave;

class StateUpdateThread : public Smart::IManagedTask {
 private:
  StateSlave *stateServer;

  bool is_running;
 public:
  StateUpdateThread(Smart::IComponent *component)
      : Smart::IManagedTask(component), stateServer(NULL), is_running(false) {}
  int on_entry() override;
  int on_exit() override;;
  virtual ~StateUpdateThread() {};
  void init(StateSlave *);

  int on_execute() override;

  void halt();

 protected:
  bool test_canceled() override;
  void sleep_for(const std::chrono::steady_clock::duration &rel_time) override;
 public:
  int start() override;
  int stop(const bool wait_till_stopped) override;

 protected:
  std::thread m_thread;
  std::atomic_bool m_interruption_requested = {false};
};

}
}
