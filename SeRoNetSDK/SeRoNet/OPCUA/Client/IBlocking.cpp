/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#include "IBlocking.hpp"

namespace SeRoNet {
namespace OPCUA {
namespace Client {
IBlocking::IBlocking(instanceStorage_t *instStorage, bool blockingEnabled)
    : m_pInstStorage(instStorage), m_it(instStorage->add(this)),
      BlockingEnabled(m_blockingEnabled), m_blockingEnabled(blockingEnabled) {
}

IBlocking::~IBlocking() {
  m_pInstStorage->remove(m_it);
}

bool IBlocking::enableBlocking() {
  // Use atomic exchange to prevent race conditions
  bool ret = m_blockingEnabled.exchange(true);
  this->blockingChanged();
  return ret;
}

bool IBlocking::disableBlocking() {
  // Use atomic exchange to prevent race conditions
  bool ret = m_blockingEnabled.exchange(false);
  this->blockingChanged();
  return ret;
}
}
}
}

