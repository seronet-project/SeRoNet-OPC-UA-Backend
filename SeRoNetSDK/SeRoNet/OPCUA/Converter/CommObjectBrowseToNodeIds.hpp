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
#include <Open62541Cpp/UA_NodeId.hpp>
#include "../../CommunicationObjects/Description/ComplexType.hpp"
#include <open62541/client.h>

namespace SeRoNet {
namespace OPCUA {
namespace Converter {

///\todo define base class for conversion <TARGET_TYPE, INPUT_TYPES ...> (Allow multi argument input)
class SERONETSDK_EXPORT CommObjectBrowseToNodeIds {
 public:
  ///
  /// \param complexType
  /// \param nodeId
  /// \param pClient only used inside the constructor
  CommObjectBrowseToNodeIds(
      CommunicationObjects::Description::IDescription::shp_t pDescription,
      const open62541Cpp::UA_NodeId& nodeId,
      std::shared_ptr<UA_Client> pClient
  );
  virtual ~CommObjectBrowseToNodeIds() = default;

  struct NodeId_NodeClass_Primitive{
    open62541Cpp::UA_NodeId NodeId;
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

}  // namespace Converter
}  // namespace OPCUA
}  // namespace SeRoNet
