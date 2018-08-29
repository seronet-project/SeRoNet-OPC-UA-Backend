///
/// \file CommObjectToUaArgument.hpp
/// \author Sebastian Friedl
/// \date 16.02.2018
///
#pragma once

#include "../../CommunicationObjects/Description/IVisitableDescription.hpp"
#include <Open62541Cpp/UA_ArrayOfArgument.hpp>
#include <open62541.h>

namespace SeRoNet {
namespace OPCUA {
namespace Converter {

///\todo define base class for conversion <TARGET_TYPE, INPUT_TYPES ...> (Allow multi argument input)
class CommObjectToUaArgumentArray {
 public:
  CommObjectToUaArgumentArray(CommunicationObjects::Description::IVisitableDescription *description);
  virtual ~CommObjectToUaArgumentArray() = default;
  typedef open62541::UA_ArrayOfArgument ConvTarget_t;
  inline  operator ConvTarget_t() { return getValue(); }
  inline ConvTarget_t getValue() { return m_convertedValue; }

 private:
  open62541::UA_ArrayOfArgument m_convertedValue;
};

} //  namespace Converter
} //  namespace OPCUA
} //  namespace SeRoNet