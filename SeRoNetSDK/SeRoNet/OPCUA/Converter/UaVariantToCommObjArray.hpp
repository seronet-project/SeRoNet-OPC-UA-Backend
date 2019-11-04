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

#include "../../CommunicationObjects/Description/ElementArray.hpp"
#include <Open62541Cpp/UA_Variant.hpp>

namespace SeRoNet {
namespace OPCUA {
namespace Converter {

class SERONETSDK_EXPORT VariantToCommObjArray {

 public:
  /// \todo use const for open62541::UA_Variant
  VariantToCommObjArray(CommunicationObjects::Description::ElementArray * arr, open62541::UA_Variant &variant);
};

} //  namespace Converter
} //  namespace OPCUA
} //  namespace SeRoNet