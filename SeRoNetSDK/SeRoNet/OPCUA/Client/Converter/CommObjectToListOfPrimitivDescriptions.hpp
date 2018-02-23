///
/// \file CommObjectToListOfPrimites.hpp
/// \author Christian von Arnim
/// \date 23.02.2018
///

#pragma once
#include <list>
#include "../../../CommunicationObjects/Description/IDescription.hpp"

namespace SeRoNet {
namespace OPCUA {
namespace Client {
namespace Converter {

/// Flat the hierarchical description to a list of primitives
class CommObjectToListOfPrimitivDescriptions {
 public:
  CommObjectToListOfPrimitivDescriptions(CommunicationObjects::Description::IDescription::shpt_t description);
  virtual ~CommObjectToUaVariantArray() = default;
  typedef std::list<CommunicationObjects::Description::IDescription::shp_t > ConvTarget_t;
  inline operator ConvTarget_t() { return getValue(); }
  inline  ConvTarget_t getValue() { return m_convertedValue; }
 private:
  ConvTarget_t m_convertedValue;
};

}
}
}
}
