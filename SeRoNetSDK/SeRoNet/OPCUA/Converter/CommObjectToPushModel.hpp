///
/// \file CommObjectToPushModell.hpp
/// \author Sebastian Friedl
/// \date 26.02.2018
///
#pragma once

#include "../../CommunicationObjects/Description/IVisitableDescription.hpp"
#include <Open62541Cpp/UA_ArrayOfArgument.hpp>
#include <open62541.h>
#include <Open62541Cpp/UA_NodeId.hpp>

namespace SeRoNet {
namespace OPCUA {
namespace Converter {

///\todo define base class for conversion <TARGET_TYPE, INPUT_TYPES ...> (Allow multi argument input)
class CommObjectToPushModel {
 public:
  CommObjectToPushModel(CommunicationObjects::Description::IVisitableDescription *description,
                        UA_Server *pServer,
                        const open62541::UA_NodeId &parent);
  virtual ~CommObjectToPushModel() = default;

 private:
  UA_Server *m_pServer;
};

} //  namespace Converter
} //  namespace OPCUA
} //  namespace SeRoNet