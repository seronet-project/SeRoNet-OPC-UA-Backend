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

#include "PushServerDisabler.hpp"

#include "../../Exceptions/NotImplementedException.hpp"
#include "../../CommunicationObjects/Description/ComplexType.hpp"
#include "../../CommunicationObjects/Description/ElementPrimitives.hpp"
#include "../../CommunicationObjects/Description/ElementArray.hpp"
#include "OpcuaServer.hpp"

/// Internal Class
class DisableOpcuaServerVisitor :
    public ::SeRoNet::CommunicationObjects::Description::IVisitorDescription {
 public:

  explicit DisableOpcuaServerVisitor(std::shared_ptr<SeRoNet::OPCUA::Server::OpcUaServer> pOpcUaServerm, const open62541Cpp::UA_NodeId &parent)
      : m_parent(parent), m_pOpcUaServer(pOpcUaServerm) {}

  void visit(SeRoNet::CommunicationObjects::Description::ComplexType *complexObject) override {
    open62541Cpp::UA_NodeId ownNodeId = generateNodeId(complexObject);

    DisableOpcuaServerVisitor visitor(m_pOpcUaServer, ownNodeId);
    for (auto &el: *complexObject) {
      el->accept(&visitor);
    }
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<bool> *el) override {
    UA_StatusCode retVal;
    auto myNodeId = generateNodeId(el);
    retVal =
        UA_Server_writeAccessLevel(m_pOpcUaServer->getServer(), *myNodeId.NodeId, 0);
    if (retVal != UA_STATUSCODE_GOOD) throw open62541Cpp::Exceptions::OpcUaErrorException(retVal);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<int8_t> *el) override {
    UA_StatusCode retVal;
    auto myNodeId = generateNodeId(el);
    retVal =
        UA_Server_writeAccessLevel(m_pOpcUaServer->getServer(), *myNodeId.NodeId, 0);
    if (retVal != UA_STATUSCODE_GOOD) throw open62541Cpp::Exceptions::OpcUaErrorException(retVal);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<int16_t> *el) override {
    UA_StatusCode retVal;
    auto myNodeId = generateNodeId(el);
    retVal =
        UA_Server_writeAccessLevel(m_pOpcUaServer->getServer(), *myNodeId.NodeId, 0);
    if (retVal != UA_STATUSCODE_GOOD) throw open62541Cpp::Exceptions::OpcUaErrorException(retVal);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<std::int32_t> *el) override {
    UA_StatusCode retVal;
    auto myNodeId = generateNodeId(el);
    retVal =
        UA_Server_writeAccessLevel(m_pOpcUaServer->getServer(), *myNodeId.NodeId, 0);
    if (retVal != UA_STATUSCODE_GOOD) throw open62541Cpp::Exceptions::OpcUaErrorException(retVal);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<int64_t> *el) override {
    UA_StatusCode retVal;
    auto myNodeId = generateNodeId(el);
    retVal =
        UA_Server_writeAccessLevel(m_pOpcUaServer->getServer(), *myNodeId.NodeId, 0);
    if (retVal != UA_STATUSCODE_GOOD) throw open62541Cpp::Exceptions::OpcUaErrorException(retVal);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<uint8_t> *el) override {
    UA_StatusCode retVal;
    auto myNodeId = generateNodeId(el);
    retVal =
        UA_Server_writeAccessLevel(m_pOpcUaServer->getServer(), *myNodeId.NodeId, 0);
    if (retVal != UA_STATUSCODE_GOOD) throw open62541Cpp::Exceptions::OpcUaErrorException(retVal);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<uint16_t> *el) override {
    UA_StatusCode retVal;
    auto myNodeId = generateNodeId(el);
    retVal =
        UA_Server_writeAccessLevel(m_pOpcUaServer->getServer(), *myNodeId.NodeId, 0);
    if (retVal != UA_STATUSCODE_GOOD) throw open62541Cpp::Exceptions::OpcUaErrorException(retVal);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<uint32_t> *el) override {
    UA_StatusCode retVal;
    auto myNodeId = generateNodeId(el);
    retVal =
        UA_Server_writeAccessLevel(m_pOpcUaServer->getServer(), *myNodeId.NodeId, 0);
    if (retVal != UA_STATUSCODE_GOOD) throw open62541Cpp::Exceptions::OpcUaErrorException(retVal);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<uint64_t> *el) override {
    UA_StatusCode retVal;
    auto myNodeId = generateNodeId(el);
    retVal =
        UA_Server_writeAccessLevel(m_pOpcUaServer->getServer(), *myNodeId.NodeId, 0);
    if (retVal != UA_STATUSCODE_GOOD) throw open62541Cpp::Exceptions::OpcUaErrorException(retVal);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<float> *el) override {
    UA_StatusCode retVal;
    auto myNodeId = generateNodeId(el);
    retVal =
        UA_Server_writeAccessLevel(m_pOpcUaServer->getServer(), *myNodeId.NodeId, 0);
    if (retVal != UA_STATUSCODE_GOOD) throw open62541Cpp::Exceptions::OpcUaErrorException(retVal);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<double> *el) override {
    UA_StatusCode retVal;
    auto myNodeId = generateNodeId(el);
    retVal =
        UA_Server_writeAccessLevel(m_pOpcUaServer->getServer(), *myNodeId.NodeId, 0);
    if (retVal != UA_STATUSCODE_GOOD) throw open62541Cpp::Exceptions::OpcUaErrorException(retVal);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<std::string> *el) override {
    UA_StatusCode retVal;
    auto myNodeId = generateNodeId(el);
    retVal =
        UA_Server_writeAccessLevel(m_pOpcUaServer->getServer(), *myNodeId.NodeId, 0);
    if (retVal != UA_STATUSCODE_GOOD) throw open62541Cpp::Exceptions::OpcUaErrorException(retVal);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementArray *elementArray) override {
    UA_StatusCode retVal;
    auto myNodeId = generateNodeId(elementArray);
    retVal = UA_Server_writeAccessLevel(m_pOpcUaServer->getServer(), *myNodeId.NodeId, 0);
    if (retVal != UA_STATUSCODE_GOOD) throw open62541Cpp::Exceptions::OpcUaErrorException(retVal);
  }

 private:

  open62541Cpp::UA_NodeId generateNodeId(const SeRoNet::CommunicationObjects::Description::IDescription *description)
  const {
    std::stringstream ss;
    switch (m_parent.NodeId->identifierType) {
      case UA_NODEIDTYPE_NUMERIC:ss << m_parent.NodeId->identifier.numeric << ".";
        break;
      case UA_NODEIDTYPE_STRING:ss << open62541Cpp::UA_String(&m_parent.NodeId->identifier.string) << ".";
        break;
      case UA_NODEIDTYPE_BYTESTRING: throw SeRoNet::Exceptions::NotImplementedException(__FUNCTION__);
      case UA_NODEIDTYPE_GUID: throw SeRoNet::Exceptions::NotImplementedException(__FUNCTION__);
    }

    ss << description->getName();
    open62541Cpp::UA_NodeId
        ownNodeId = open62541Cpp::UA_NodeId(m_pOpcUaServer->getNsIndex1(), ss.str());
    return ownNodeId;
  }

  std::shared_ptr<SeRoNet::OPCUA::Server::OpcUaServer> m_pOpcUaServer;
  open62541Cpp::UA_NodeId m_parent;
};

namespace SeRoNet {
namespace OPCUA {
namespace Server {

/// @TODO (Sebastian Friedl) add Status Message for Error Handling
PushServerDisabler::PushServerDisabler(
    std::shared_ptr<SeRoNet::OPCUA::Server::OpcUaServer> pOpcUaServer,
    CommunicationObjects::Description::IVisitableDescription *description,
    const open62541Cpp::UA_NodeId &parent) {
  DisableOpcuaServerVisitor visitor(pOpcUaServer, parent);
  description->accept(&visitor);
}

}
}
}
