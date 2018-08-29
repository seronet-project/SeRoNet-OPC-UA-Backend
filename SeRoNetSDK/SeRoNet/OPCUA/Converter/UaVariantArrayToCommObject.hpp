///
/// \file UaVariantArrayToCommObject.hpp
/// \author Christian von Arnim
/// \date 16.02.2018
///

#pragma once

#include <Open62541Cpp/UA_ArrayOfVariant.hpp>
#include "../../CommunicationObjects/Description/IVisitableDescription.hpp"

namespace SeRoNet {
namespace OPCUA {
namespace Converter {


class UaVariantArrayToCommObject {
 public:
  UaVariantArrayToCommObject(open62541::UA_ArrayOfVariant src, CommunicationObjects::Description::IVisitableDescription *target);
  virtual ~UaVariantArrayToCommObject() = default;
 private:

};

} //  namespace Converter
} //  namespace OPCUA
} //  namespace SeRoNet
