/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#pragma once

#include <thread>
#include <atomic>

#include <smartIComponent.h>
#include <smartITask.h>
#include "../Exceptions/NotImplementedException.hpp"

namespace SeRoNet {

namespace Utils {

class Task :
    public Smart::ITask {
 private:
  std::thread m_thread;

  std::atomic_bool m_interruption_requested = {false};

 protected:
  void sleep_for(const std::chrono::steady_clock::duration &rel_time) override {
    throw SeRoNet::Exceptions::NotImplementedException(__FUNCTION__);
  }

 public:
  using Smart::ITask::ITask;

  ~Task() override {
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
   * @warning wait_till_stopped=false not implemented
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

}//  namespace Utils
}//  namespace SeRoNet
