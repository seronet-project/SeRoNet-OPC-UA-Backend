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

#include <list>
#include "../../CommunicationObjects/Description/IDescription.hpp"

namespace SeRoNet {
namespace OPCUA {
namespace Converter {

/// Flat the hierarchical description to a list of primitives
class SERONETSDK_EXPORT CommObjectToListOfPrimitivDescriptions {
 public:
  CommObjectToListOfPrimitivDescriptions(CommunicationObjects::Description::IDescription::shp_t description);
  virtual ~CommObjectToListOfPrimitivDescriptions() = default;
  typedef std::list<CommunicationObjects::Description::IDescription::shp_t > ConvTarget_t;
  inline operator ConvTarget_t() { return getValue(); }
  inline  ConvTarget_t getValue() { return m_convertedValue; }
 private:
  ConvTarget_t m_convertedValue;
};

}  // namespace Converter
}  // namespace OPCUA
}  // namespace SeRoNet
