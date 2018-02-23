///
/// \file CommObjectBrowseToNodeIds.hpp
/// \author Christian von Arnim
/// \date 21.02.2018
///

#pragma once

#include <list>
#include <Open62541Cpp/UA_NodeId.hpp>
#include "../../../CommunicationObjects/Description/ComplexType.hpp"

namespace SeRoNet {
namespace OPCUA {
namespace Client {
namespace Converter {

///\todo define base class for conversion <TARGET_TYPE, INPUT_TYPES ...> (Allow multi argument input)
class CommObjectBrowseToNodeIds {
 public:
  ///
  /// \param complexType
  /// \param nodeId
  /// \param pClient only used inside the constructor
  CommObjectBrowseToNodeIds(
      CommunicationObjects::Description::ComplexType::shp_t complexType,
      const open62541::UA_NodeId& nodeId,
      std::shared_ptr<UA_Client> pClient
  );
  virtual ~CommObjectToUaVariantArray() = default;

  struct NodeId_NodeClass_Primitive{
    open62541::UA_NodeId NodeId;
    CommunicationObjects::Description::IDescription::shp_t Description;
    UA_NodeClass NodeClass;
  };

  typedef std::list<NodeId_NodeClass_Primitive> ConvTarget_t;
  inline operator ConvTarget_t() { return getValue(); }
  inline  ConvTarget_t getValue() { return m_convertedValue; }
 private:
  ///\todo define static
  CommunicationObjects::Description::IDescription::shp_t getDescriptionByName(
      std::string name,
      CommunicationObjects::Description::ComplexType::shp_t complexType
  );
  ConvTarget_t m_convertedValue;
};

}
}
}
}
