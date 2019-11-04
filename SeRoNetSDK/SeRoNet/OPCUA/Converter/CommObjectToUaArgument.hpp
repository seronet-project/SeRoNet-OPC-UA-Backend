/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#pragma once

#include "../../../Definitions.hpp"
#include "../../CommunicationObjects/Description/IVisitableDescription.hpp"
#include <Open62541Cpp/UA_ArrayOfArgument.hpp>
#include <open62541/types_generated.h>

namespace SeRoNet {
namespace OPCUA {
namespace Converter {

///\todo define base class for conversion <TARGET_TYPE, INPUT_TYPES ...> (Allow multi argument input)
class SERONETSDK_EXPORT CommObjectToUaArgumentArray {
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