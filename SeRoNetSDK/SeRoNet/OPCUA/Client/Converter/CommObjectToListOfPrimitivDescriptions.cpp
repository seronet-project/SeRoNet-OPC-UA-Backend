///
/// \file CommObjectToListOfPrimites.cpp
/// \author Christian von Arnim
/// \date 23.02.2018
///

#include "CommObjectToListOfPrimitivDescriptions.hpp"

namespace SeRoNet {
namespace OPCUA {
namespace Client {
namespace Converter {

CommObjectToListOfPrimitivDescriptions::CommObjectToListOfPrimitivDescriptions(
    CommunicationObjects::Description::IDescription::shpt_t description) {

  auto complexOj = std::dynamic_pointer_cast<CommunicationObjects::Description::ComplexType>(description);
  if (complexOj) {
    //Complex object, flat it to list
    for (auto &element: *complexOj) {
      CommObjectToListOfPrimitivDescriptions elementList(element);
      m_convertedValue.splice(m_convertedValue.end(), elementList.m_convertedValue);
    }
  } else {
    // primitive type, just add it to the list
    m_convertedValue.push_back(description);
  }

}

}
}
}
}
