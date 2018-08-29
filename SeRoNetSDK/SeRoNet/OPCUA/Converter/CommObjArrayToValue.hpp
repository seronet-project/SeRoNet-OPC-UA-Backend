///
/// \file CommObjArrayToValue.hpp
/// \author Christian von Arnim
/// \date 03.08.2018
///


#include <open62541/open62541.h>
#include "../../CommunicationObjects/Description/ElementArray.hpp"

#pragma once
namespace SeRoNet {
namespace OPCUA {
namespace Converter {

class CommObjArrayToValue {
 public:
  CommObjArrayToValue(CommunicationObjects::Description::ElementArray *arr);
  UA_Variant Value();
  virtual ~CommObjArrayToValue();
 protected:
  UA_Variant m_variant;
};

}  // namespace Converter
}  // namespace OPCUA
}  // namespace SeRoNet
