///
/// \file PushServerUpdater.hpp
/// \author Sebastian Friedl
/// \date 27.02.2018
///
#pragma once

#include "../../CommunicationObjects/Description/IVisitableDescription.hpp"
#include <Open62541Cpp/UA_ArrayOfArgument.hpp>
#include <open62541.h>
#include <Open62541Cpp/UA_NodeId.hpp>

namespace SeRoNet {
namespace OPCUA {
namespace Server {

///\todo define base class for conversion <TARGET_TYPE, INPUT_TYPES ...> (Allow multi argument input)
class PushServerUpdater {
 public:
  PushServerUpdater(CommunicationObjects::Description::IVisitableDescription *description,
                    UA_Server *pServer,
                    const OPEN_65241_CPP_NAMESPACE::UA_NodeId &parent,
                    UA_UInt16 nsIndex);
  virtual ~PushServerUpdater() = default;

 private:
  UA_UInt16 m_nsIndex;
  UA_Server *m_pServer;
};

} //  namespace Server
} //  namespace OPCUA
} //  namespace SeRoNet