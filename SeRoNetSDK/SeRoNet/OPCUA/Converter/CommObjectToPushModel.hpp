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
#include "../../CommunicationObjects/Description/IDescription.hpp"
#include <Open62541Cpp/UA_ArrayOfArgument.hpp>
#include "../../OPCUA/Server/OpcuaServer.hpp"
#include <Open62541Cpp/UA_NodeId.hpp>

namespace SeRoNet {
namespace OPCUA {
namespace Converter {

///\todo define base class for conversion <TARGET_TYPE, INPUT_TYPES ...> (Allow multi argument input)
class SERONETSDK_EXPORT CommObjectToPushModel {
 public:
  CommObjectToPushModel(
      std::shared_ptr<SeRoNet::OPCUA::Server::OpcUaServer> pServer,
      CommunicationObjects::Description::IDescription *description,
      const open62541Cpp::UA_NodeId &parent);
  virtual ~CommObjectToPushModel() = default;

  open62541Cpp::UA_NodeId GetRootNodeId() const;

 private:
  std::shared_ptr<SeRoNet::OPCUA::Server::OpcUaServer> m_pServer;
  open62541Cpp::UA_NodeId m_rootNodeId;
};

} //  namespace Converter
} //  namespace OPCUA
} //  namespace SeRoNet