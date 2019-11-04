/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/


#include <list>
#include <sstream>

#include <Open62541Cpp/UA_Argument.hpp>
#include <Open62541Cpp/UA_String.hpp>
#include <Open62541Cpp/UA_NodeId.hpp>
#include <Open62541Cpp/Exceptions/OpcUaErrorException.hpp>

#include <Open62541Cpp/UA_Variant.hpp>
#include <iostream>
#include <cstdint>

#include "PushServerEnabler.hpp"

#include "../../Exceptions/NotImplementedException.hpp"
#include "../../CommunicationObjects/Description/ComplexType.hpp"
#include "../../CommunicationObjects/Description/ElementPrimitives.hpp"
#include "../../CommunicationObjects/Description/ElementArray.hpp"
#include "OpcuaServer.hpp"

/// Internal Class
class EnableOpcuaServerVisitor :
    public ::SeRoNet::CommunicationObjects::Description::IVisitorDescription {
 public:

  explicit EnableOpcuaServerVisitor(std::shared_ptr<SeRoNet::OPCUA::Server::OpcUaServer> pOpcUaServer, const open62541::UA_NodeId &parent)
      : m_pOpcUaServer(pOpcUaServer), m_parent(parent) {}

  void visit(SeRoNet::CommunicationObjects::Description::ComplexType *complexObject) override {
    open62541::UA_NodeId ownNodeId = generateNodeId(complexObject);

    EnableOpcuaServerVisitor visitor(m_pOpcUaServer, ownNodeId);
    for (auto &el: *complexObject) {
      el->accept(&visitor);
    }
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<bool> *el) override {
    allowReadAccess(el);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<int8_t> *el) override {
    allowReadAccess(el);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<int16_t> *el) override {
    allowReadAccess(el);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<int32_t> *el) override {
    allowReadAccess(el);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<int64_t> *el) override {
    allowReadAccess(el);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<uint8_t> *el) override {
    allowReadAccess(el);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<uint16_t> *el) override {
    allowReadAccess(el);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<uint32_t> *el) override {
    allowReadAccess(el);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<uint64_t> *el) override {
    allowReadAccess(el);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<float> *el) override {
    allowReadAccess(el);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<double> *el) override {
    allowReadAccess(el);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<std::string> *el) override {
    allowReadAccess(el);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementArray *elementArray) override {
    allowReadAccess(elementArray);
  }

 private:

  void allowReadAccess(const SeRoNet::CommunicationObjects::Description::IDescription *description)
  {
    UA_StatusCode retVal;
    auto myNodeId = generateNodeId(description);
    retVal = UA_Server_writeAccessLevel(m_pOpcUaServer->getServer(),
                                        *myNodeId.NodeId,
                                        UA_ACCESSLEVELMASK_READ);
    if (retVal != UA_STATUSCODE_GOOD) throw open62541::Exceptions::OpcUaErrorException(retVal);
  }

  open62541::UA_NodeId generateNodeId(const SeRoNet::CommunicationObjects::Description::IDescription *description)
  const {
    std::stringstream ss;
    switch (m_parent.NodeId->identifierType) {
      case UA_NODEIDTYPE_NUMERIC:ss << m_parent.NodeId->identifier.numeric << ".";
        break;
      case UA_NODEIDTYPE_STRING:ss << open62541::UA_String(&m_parent.NodeId->identifier.string) << ".";
        break;
      case UA_NODEIDTYPE_BYTESTRING: throw SeRoNet::Exceptions::NotImplementedException(__FUNCTION__);
      case UA_NODEIDTYPE_GUID: throw SeRoNet::Exceptions::NotImplementedException(__FUNCTION__);
    }

    ss << description->getName();
    open62541::UA_NodeId
        ownNodeId = open62541::UA_NodeId(m_pOpcUaServer->getNsIndex1(), ss.str());
    return ownNodeId;
  }
  std::shared_ptr<SeRoNet::OPCUA::Server::OpcUaServer> m_pOpcUaServer;
  open62541::UA_NodeId m_parent;
};

namespace SeRoNet {
namespace OPCUA {
namespace Server {

/// @TODO (Sebastian Friedl) add Status Message for Error Handling
PushServerEnabler::PushServerEnabler(
    std::shared_ptr<SeRoNet::OPCUA::Server::OpcUaServer> pOpcUaServer,
    CommunicationObjects::Description::IVisitableDescription *description,
    const open62541::UA_NodeId &parent) {
  EnableOpcuaServerVisitor visitor(pOpcUaServer, parent);
  description->accept(&visitor);
}

}
}
}
