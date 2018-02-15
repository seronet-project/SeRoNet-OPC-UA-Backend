/**
* @author Sebastian Friedl
*/

#ifndef OPCUASERONETSDK_TASKIMPL_H_
#define OPCUASERONETSDK_TASKIMPL_H_

#include <thread>
#include <atomic>

#include "../../../SmartSoftComponentDeveloperAPIcpp/SmartSoft_CD_API/smartIComponent.h"
#include "../../../SmartSoftComponentDeveloperAPIcpp/SmartSoft_CD_API/smartITask.h"

namespace SeRoNet {

namespace Utils {

class Task : public Smart::ITask {
 private:
  std::thread m_thread;

  std::atomic_bool m_interruption_requested = false;

 public:
  using Smart::ITask::ITask;

  virtual ~Task() {
    this->close();
  }

  /** Creates and starts a new thread (if not yet started)
   *
   */
  virtual int open() {
    m_thread = std::thread(&Task::svc, this);
    return 0;
  }

  /** Closes currently active thread (if it was started before)
   */
  virtual int close() {
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
  virtual bool test_canceled() {
    return this->m_interruption_requested.load();
  }
};

} /* namespace utils */
} /* namespace SeRoNet */

#endif /* OPCUASERONETSDK_FREEOPCUAIMPL_SMARTTASKIMPL_H_ */
