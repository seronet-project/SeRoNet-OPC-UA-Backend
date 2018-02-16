///
/// \file CoordinateObjectToUaVariantArray.hpp
/// \author Christian von Arnim
/// \date 16.02.2018
///

#pragma once

#include <Open62541Cpp/UA_ArrayOfVariant.hpp>
#include "../../../CommunicationObjects/Description/IVisitableDescription.hpp"

namespace SeRoNet {
namespace OPCUA {
namespace Client {
namespace Converter {

///\todo define base class for conversion <TARGET_TYPE, INPUT_TYPES ...> (Allow multi argument input)
class CoordinateObjectToUaVariantArray {
 public:
  CoordinateObjectToUaVariantArray(CommunicationObjects::Description::IVisitableDescription *description);
  virtual ~CoordinateObjectToUaVariantArray() = default;
  typedef open62541::UA_ArrayOfVariant ConvTarget_t;
  inline operator ConvTarget_t() { return getValue(); }
  inline  ConvTarget_t getValue() { return m_convertedValue; }
 private:
  open62541::UA_ArrayOfVariant m_convertedValue;
};

}
}
}
}
