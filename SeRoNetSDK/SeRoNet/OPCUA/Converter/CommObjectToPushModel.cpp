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
#include <open62541/types_generated.h>
#include <iostream>
#include <cstdint>

#include "CommObjectToPushModel.hpp"
#include "../../Exceptions/NotImplementedException.hpp"
#include "../../CommunicationObjects/Description/ComplexType.hpp"
#include "../../CommunicationObjects/Description/ElementPrimitive.hpp"
#include "../../CommunicationObjects/Description/ElementArray.hpp"
#include "../Server/OpcuaServer.hpp"

/// Internal Class
class ToPushModellVisitor :
    public ::SeRoNet::CommunicationObjects::Description::IVisitorDescription {
 public:

  ToPushModellVisitor(std::shared_ptr<SeRoNet::OPCUA::Server::OpcUaServer> pServer,
                      const open62541Cpp::UA_NodeId &parent)
      : m_pServer(pServer),
        m_parent(parent) {}

  /// WARNING this Methode is untested
  void visit(SeRoNet::CommunicationObjects::Description::ComplexType *complexObject) override {
    const open62541Cpp::UA_NodeId ownNodeId = generateNodeId(complexObject);
    UA_ObjectAttributes attr = UA_ObjectAttributes_default;
    attr.displayName = UA_LOCALIZEDTEXT_ALLOC("", complexObject->getName().c_str());

    UA_QualifiedName qualifiedName = UA_QUALIFIEDNAME_ALLOC(m_pServer->getNsIndex1(), complexObject->getName().c_str());

    UA_StatusCode retVal = UA_Server_addObjectNode(
        m_pServer->getServer(),
        *ownNodeId.NodeId,
        *m_parent.NodeId,
        UA_NODEID_NUMERIC(m_pServer->getNsIndex0(), UA_NS0ID_ORGANIZES),
        qualifiedName,
        UA_NODEID_NUMERIC(m_pServer->getNsIndex0(), UA_NS0ID_BASEOBJECTTYPE),
        attr,
        nullptr,
        nullptr
    );

    UA_QualifiedName_clear(&qualifiedName);
    UA_ObjectAttributes_clear(&attr);

    if (retVal != UA_STATUSCODE_GOOD) throw open62541Cpp::Exceptions::OpcUaErrorException(retVal);

    ToPushModellVisitor visitor(m_pServer, ownNodeId);
    for (auto &el: *complexObject) {
      el->accept(&visitor);
    }
  }

  void createPrimitive(SeRoNet::CommunicationObjects::Description::IDescription *el, std::size_t uaTypesIndex) {
    UA_StatusCode retVal;
    UA_VariableAttributes attr = UA_VariableAttributes_default;

    attr.userAccessLevel = UA_ACCESSLEVELMASK_READ;
    attr.accessLevel = UA_ACCESSLEVELMASK_READ;
    UA_NodeId_copy(&UA_TYPES[uaTypesIndex].typeId, &attr.dataType);
    UA_init(&attr.value, &UA_TYPES[uaTypesIndex]);
    attr.displayName = UA_LOCALIZEDTEXT_ALLOC("", el->getName().c_str());

    auto myNodeId = generateNodeId(el);

    UA_QualifiedName qualifiedName = UA_QUALIFIEDNAME_ALLOC(m_pServer->getNsIndex1(), el->getName().c_str());

    retVal = UA_Server_addVariableNode(
        m_pServer->getServer(),
        *myNodeId.NodeId,
        *m_parent.NodeId,
        UA_NODEID_NUMERIC(m_pServer->getNsIndex0(), UA_NS0ID_HASCOMPONENT),
        qualifiedName,
        UA_NODEID_NUMERIC(m_pServer->getNsIndex0(), UA_NS0ID_BASEDATAVARIABLETYPE),
        attr,
        nullptr,
        nullptr
    );

    UA_QualifiedName_clear(&qualifiedName);
    UA_VariableAttributes_clear(&attr);
    if (retVal != UA_STATUSCODE_GOOD) throw open62541Cpp::Exceptions::OpcUaErrorException(retVal);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<bool> *el) override {

    createPrimitive(el, UA_TYPES_BOOLEAN);
    disableElement(el);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<std::int8_t> *el) override {
    createPrimitive(el, UA_TYPES_BYTE);
    disableElement(el);
  }
  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<std::int16_t> *el) override {
    createPrimitive(el, UA_TYPES_INT16);
    disableElement(el);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<std::int32_t> *el) override {
    createPrimitive(el, UA_TYPES_INT32);
    disableElement(el);
  }
  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<std::int64_t> *el) override {
    createPrimitive(el, UA_TYPES_INT64);
    disableElement(el);
  }
  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<std::uint8_t> *el) override {
    createPrimitive(el, UA_TYPES_BYTE);
    disableElement(el);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<std::uint16_t> *el) override {
    createPrimitive(el, UA_TYPES_UINT16);
    disableElement(el);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<std::uint32_t> *el) override {
    createPrimitive(el, UA_TYPES_UINT32);
    disableElement(el);
  }
  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<std::uint64_t> *el) override {
    createPrimitive(el, UA_TYPES_UINT64);
    disableElement(el);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<float> *el) override {
    createPrimitive(el, UA_TYPES_FLOAT);
    disableElement(el);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<double> *el) override {
    createPrimitive(el, UA_TYPES_DOUBLE);
    disableElement(el);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<std::string> *el) override {
    createPrimitive(el, UA_TYPES_STRING);
    disableElement(el);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementArray *elementArray) override {
    createPrimitive(static_cast<SeRoNet::CommunicationObjects::Description::IDescription*> (elementArray), UA_TYPES_VARIANT);
    disableElement(elementArray);
  }

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
        ownNodeId = open62541Cpp::UA_NodeId(m_pServer->getNsIndex1(), ss.str());
    return ownNodeId;
  }

 private:

  void disableElement(const SeRoNet::CommunicationObjects::Description::IDescription *el) const {
    UA_StatusCode retVal;
    auto myNodeId = generateNodeId(el);
    retVal = UA_Server_writeAccessLevel(m_pServer->getServer(),
                                        *myNodeId.NodeId,
                                        0);
    if (retVal != UA_STATUSCODE_GOOD) throw open62541Cpp::Exceptions::OpcUaErrorException(retVal);
  }

  std::shared_ptr<SeRoNet::OPCUA::Server::OpcUaServer> m_pServer;
  open62541Cpp::UA_NodeId m_parent;
};

namespace SeRoNet {
namespace OPCUA {
namespace Converter {

CommObjectToPushModel::CommObjectToPushModel(
    std::shared_ptr<SeRoNet::OPCUA::Server::OpcUaServer> pServer,
    CommunicationObjects::Description::IDescription *description,
    const open62541Cpp::UA_NodeId &parent) : m_pServer(pServer) {
  ToPushModellVisitor visitor(pServer, parent);
  m_rootNodeId = visitor.generateNodeId(description);
  description->accept(&visitor);
}

open62541Cpp::UA_NodeId CommObjectToPushModel::GetRootNodeId() const {
  return m_rootNodeId;
}

}  // namespace Converter
}  // namespace OPCUA
}  // namespace SeRoNet
