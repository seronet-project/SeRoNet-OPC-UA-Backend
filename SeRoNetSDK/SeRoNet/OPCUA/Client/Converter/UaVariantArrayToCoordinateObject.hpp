///
/// \file UaVariantArrayToCoordinateObject.hpp
/// \author Christian von Arnim
/// \date 16.02.2018
///

#pragma once

#include <Open62541Cpp/UA_ArrayOfVariant.hpp>
#include "../../../CommunicationObjects/Description/IVisitableDescription.hpp"

#include <Open62541Cpp/UA_ArrayOfVariant.hpp>
#include "../../../CommunicationObjects/Description/IVisitableDescription.hpp"

namespace SeRoNet {
namespace OPCUA {
namespace Client {
namespace Converter {


class UaVariantArrayToCoordinateObject {
 public:
  UaVariantArrayToCoordinateObject(open62541::UA_ArrayOfVariant src, CommunicationObjects::Description::IVisitableDescription *target);
  virtual ~UaVariantArrayToCoordinateObject() = default;
 private:

};

}
}
}
}


