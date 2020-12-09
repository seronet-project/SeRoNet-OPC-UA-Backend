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


class SERONETSDK_EXPORT UaVariantArrayToCommObject {
 public:
  UaVariantArrayToCommObject(open62541Cpp::UA_ArrayOfVariant src, CommunicationObjects::Description::IVisitableDescription *target);
  virtual ~UaVariantArrayToCommObject() = default;
 private:

};

} //  namespace Converter
} //  namespace OPCUA
} //  namespace SeRoNet
