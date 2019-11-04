/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#include "../../../Definitions.hpp"

#include <open62541/types.h>
#include "../../CommunicationObjects/Description/ElementArray.hpp"

#pragma once
namespace SeRoNet {
namespace OPCUA {
namespace Converter {

class SERONETSDK_EXPORT CommObjArrayToValue {
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
