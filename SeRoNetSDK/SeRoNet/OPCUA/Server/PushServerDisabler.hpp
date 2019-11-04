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
#include "../../CommunicationObjects/Description/IVisitableDescription.hpp"
#include <Open62541Cpp/UA_ArrayOfArgument.hpp>
#include "OpcuaServer.hpp"
#include <Open62541Cpp/UA_NodeId.hpp>

namespace SeRoNet {
namespace OPCUA {
namespace Server {

///\todo define base class for conversion <TARGET_TYPE, INPUT_TYPES ...> (Allow multi argument input)
class SERONETSDK_EXPORT PushServerDisabler {
 public:
  PushServerDisabler(
      std::shared_ptr<SeRoNet::OPCUA::Server::OpcUaServer> m_pOpcUaServer,
      CommunicationObjects::Description::IVisitableDescription *description,
                     const OPEN_65241_CPP_NAMESPACE::UA_NodeId &parent);

  virtual ~PushServerDisabler() = default;
};

} //  namespace Server
} //  namespace OPCUA
} //  namespace SeRoNet