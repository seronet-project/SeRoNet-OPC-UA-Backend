/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#include "CommObjectToListOfPrimitivDescriptions.hpp"
#include "../../CommunicationObjects/Description/ComplexType.hpp"

namespace SeRoNet {
namespace OPCUA {
namespace Converter {

CommObjectToListOfPrimitivDescriptions::CommObjectToListOfPrimitivDescriptions(
    CommunicationObjects::Description::IDescription::shp_t description) {

  auto complexObj = std::dynamic_pointer_cast<CommunicationObjects::Description::ComplexType>(description);
  if (complexObj) {
    //Complex object, flat it to list
    //for (auto element = complexObj->begin(); element != complexObj->end(); ++element) {
    for (auto &element : *complexObj) {
      CommObjectToListOfPrimitivDescriptions elementList(element);
      m_convertedValue.splice(m_convertedValue.end(), elementList.m_convertedValue);
    }
  } else {
    // primitive type, just add it to the list
    m_convertedValue.push_back(description);
  }

}

}  // namespace Converter
}  // namespace OPCUA
}  // namespace SeRoNet
