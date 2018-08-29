///
/// \file ValueToCommObjArray.hpp
/// \author Christian von Arnim
/// \date 09.08.2018
///

#pragma once

#include "../../CommunicationObjects/Description/ElementArray.hpp"
#include <Open62541Cpp/UA_Variant.hpp>

namespace SeRoNet {
namespace OPCUA {
namespace Converter {

class VariantToCommObjArray {

 public:
  /// \todo use const for open62541::UA_Variant
  VariantToCommObjArray(CommunicationObjects::Description::ElementArray * arr, open62541::UA_Variant &variant);
};

} //  namespace Converter
} //  namespace OPCUA
} //  namespace SeRoNet