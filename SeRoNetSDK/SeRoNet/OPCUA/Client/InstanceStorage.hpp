/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/


#ifndef SERONETSDK_INSTANCESTORAGE_HPP
#define SERONETSDK_INSTANCESTORAGE_HPP

#include <list>

#include <functional>
#include <mutex>

namespace SeRoNet {
namespace OPCUA {
namespace Client {
template<typename INSTANCE_TYPE_T>
class InstanceStorage {
 public:
  // Must be a "list" type, which keep iterators valid when editing the container
  // (see also http://kera.name/articles/2011/06/iterator-invalidation-rules-c0x/)
  typedef std::list<INSTANCE_TYPE_T *> Instance_Container_t;
  typedef typename Instance_Container_t::iterator Instance_Container_Iterator_t;

  Instance_Container_Iterator_t add(INSTANCE_TYPE_T *inst);
  void remove(Instance_Container_Iterator_t &it);

  typedef std::function<void(INSTANCE_TYPE_T *)> foreach_callback_t;
  /// @return number of elements
  int foreach(foreach_callback_t callb);

  bool empty() { return m_instances.empty(); }
 protected:
  Instance_Container_t m_instances;
  std::mutex m_accessMutex;
};

template<typename INSTANCE_TYPE_T>
inline typename InstanceStorage<INSTANCE_TYPE_T>::Instance_Container_Iterator_t InstanceStorage<INSTANCE_TYPE_T>::add(
    INSTANCE_TYPE_T *inst) {
  std::unique_lock<decltype(m_accessMutex)> lock;
  return m_instances.insert(m_instances.end(), inst);
}

template<typename INSTANCE_TYPE_T>
inline void InstanceStorage<INSTANCE_TYPE_T>::remove(InstanceStorage<INSTANCE_TYPE_T>::Instance_Container_Iterator_t &it) {
  std::unique_lock<decltype(m_accessMutex)> lock;
  m_instances.erase(it);
}

template<typename INSTANCE_TYPE_T>
inline int InstanceStorage<INSTANCE_TYPE_T>::foreach(foreach_callback_t callb) {
  std::unique_lock<decltype(m_accessMutex)> lock;
  for (auto &inst: m_instances) {
    callb(inst);
  }
  return m_instances.size();
}
}
}
}

#endif //SERONETSDK_INSTANCESTORAGE_HPP
