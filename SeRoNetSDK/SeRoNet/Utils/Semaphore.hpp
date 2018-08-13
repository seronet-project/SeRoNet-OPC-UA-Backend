//
// Created by ac120371 on 09.08.2018.
//

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
