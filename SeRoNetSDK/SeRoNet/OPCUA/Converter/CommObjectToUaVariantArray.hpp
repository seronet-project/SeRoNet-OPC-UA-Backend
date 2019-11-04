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

#include <Open62541Cpp/UA_ArrayOfVariant.hpp>
#include "../../CommunicationObjects/Description/IVisitableDescription.hpp"

namespace SeRoNet {
namespace OPCUA {
namespace Converter {

///\todo define base class for conversion <TARGET_TYPE, INPUT_TYPES ...> (Allow multi argument input)
class SERONETSDK_EXPORT CommObjectToUaVariantArray {
 public:
  CommObjectToUaVariantArray(CommunicationObjects::Description::IVisitableDescription *description);
  virtual ~CommObjectToUaVariantArray() = default;
  typedef open62541::UA_ArrayOfVariant ConvTarget_t;
  inline operator ConvTarget_t() { return getValue(); }
  inline  ConvTarget_t getValue() { return m_convertedValue; }
 private:
  open62541::UA_ArrayOfVariant m_convertedValue;
};

} //  namespace Converter
} //  namespace OPCUA
} //  namespace SeRoNet