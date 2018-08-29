///
/// \file CommObjectToListOfPrimites.cpp
/// \author Christian von Arnim
/// \date 23.02.2018
///

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
