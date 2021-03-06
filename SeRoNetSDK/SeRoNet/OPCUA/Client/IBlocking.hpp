/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/


#ifndef SERONETSDK_IBLOCKING_HPP
#define SERONETSDK_IBLOCKING_HPP

#include "../../../Definitions.hpp"
#include "InstanceStorage.hpp"
#include <atomic>

namespace SeRoNet {
namespace OPCUA {
namespace Client {
class SERONETSDK_EXPORT IBlocking {
 public:
  typedef InstanceStorage<IBlocking> instanceStorage_t;
  ///@TODO use shared ptr?! for instStorage
  explicit IBlocking(instanceStorage_t *instStorage, bool blockingEnabled);

  ///@return old value of blockingEnabled
  bool enableBlocking();

  ///@return old value of blockingEnabled
  bool disableBlocking();

  bool isBlockingEnabled() { return m_blockingEnabled; };

  virtual ~IBlocking();
 protected:
  /// Called when the value of BlockingEnabled is changed
  virtual void blockingChanged() = 0;
  /// ReadOnly m_blockingEnabled
  const std::atomic_bool &BlockingEnabled;
 private:
  std::atomic_bool m_blockingEnabled = {true};
  instanceStorage_t *m_pInstStorage;
  instanceStorage_t::Instance_Container_Iterator_t m_it;
};
}
}
}

#endif //SERONETSDK_IBLOCKING_HPP
