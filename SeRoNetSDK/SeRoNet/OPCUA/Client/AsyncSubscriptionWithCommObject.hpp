///
/// \file AsyncSubscriptionWithCommObject.hpp
/// \author Christian von Arnim
/// \date 21.02.2018
///

#pragma once

#include "AsyncSubscriptionOpcUa.hpp"
#include <Open62541Cpp/UA_NodeId.hpp>

namespace SeRoNet {
namespace OPCUA {
namespace Client {

template<typename T_DATATYPE>
class AsyncSubscriptionWithCommObject : public AsyncSubscriptionOpcUa<T_DATATYPE>{
 public:
  using AsyncSubscriptionOpcUa<T_DATATYPE>::AsyncSubscriptionOpcUa;
  /// Subscribe based on an CommunicationObject description
  /// \return UA_GOOD in case of no error
  virtual UA_StatusCode subscribe(const open62541::UA_NodeId& nodeId);
};

template<typename T_DATATYPE>
UA_StatusCode AsyncSubscriptionWithCommObject<T_DATATYPE>::subscribe(const open62541::UA_NodeId& nodeId) {
  ///\todo
}

}
}
}
