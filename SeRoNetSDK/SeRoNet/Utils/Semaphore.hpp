/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#ifndef SERONETSDK_SEMAPHORE_HPP
#define SERONETSDK_SEMAPHORE_HPP

#include <mutex>
#include <condition_variable>
namespace SeRoNet {
namespace Utils {

// based on https://stackoverflow.com/questions/4792449/c0x-has-no-semaphores-how-to-synchronize-threads
class Semaphore {
 public:
  Semaphore(int count_ = 0)
      : count(count_) {}

  inline void notify() {
    std::unique_lock<std::mutex> lock(mtx);
    count++;
    cv.notify_one();
  }

  inline void wait() {
    std::unique_lock<std::mutex> lock(mtx);

    while (count == 0) {
      cv.wait(lock);
    }
    count--;
  }

 private:
  std::mutex mtx;
  std::condition_variable cv;
  int count;
};

}
}

#endif //SERONETSDK_SEMAPHORE_HPP
