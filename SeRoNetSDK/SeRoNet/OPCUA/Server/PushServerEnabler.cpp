///
/// \file PushServerDisabler.cpp
/// \author Sebastian Friedl
/// \date  12.09.2018
///

#include <list>
#include <sstream>

#include <Open62541Cpp/UA_Argument.hpp>
#include <Open62541Cpp/UA_String.hpp>
#include <Open62541Cpp/UA_NodeId.hpp>
#include <Open62541Cpp/Exceptions/OpcUaErrorException.hpp>

//#include <open62541.h>
#include <Open62541Cpp/UA_Variant.hpp>
#include <iostream>
#include <cstdint>

#include "PushServerDisabler.hpp"

#include "../../Exceptions/NotImplementedException.hpp"
#include "../../CommunicationObjects/Description/ComplexType.hpp"
#include "../../CommunicationObjects/Description/ElementPrimitives.hpp"
#include "OpcuaServer.hpp"

/// Internal Class
class UpdateOpcuaServerVisitor :
    public ::SeRoNet::CommunicationObjects::Description::IVisitorDescription {
 public:

  explicit UpdateOpcuaServerVisitor(const open62541::UA_NodeId &parent)
      : m_parent(parent) {}

  void visit(SeRoNet::CommunicationObjects::Description::ComplexType *complexObject) override {
    open62541::UA_NodeId ownNodeId = generateNodeId(complexObject);

    UpdateOpcuaServerVisitor visitor(ownNodeId);
    for (auto &el: *complexObject) {
      el->accept(&visitor);
    }
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<bool> *el) override {
    UA_StatusCode retVal;
    auto myNodeId = generateNodeId(el);
    retVal = UA_Server_writeAccessLevel(SeRoNet::OPCUA::Server::OpcUaServer::instance().getServer(),*myNodeId.NodeId,0);
    if (retVal != UA_STATUSCODE_GOOD) throw open62541::Exceptions::OpcUaErrorException(retVal);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<std::int32_t> *el) override {
    UA_StatusCode retVal;
    auto myNodeId = generateNodeId(el);
    retVal = UA_Server_writeAccessLevel(SeRoNet::OPCUA::Server::OpcUaServer::instance().getServer(),*myNodeId.NodeId,0);
    if (retVal != UA_STATUSCODE_GOOD) throw open62541::Exceptions::OpcUaErrorException(retVal);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<float> *el) override {
    UA_StatusCode retVal;
    auto myNodeId = generateNodeId(el);
    retVal = UA_Server_writeAccessLevel(SeRoNet::OPCUA::Server::OpcUaServer::instance().getServer(),*myNodeId.NodeId,0);
    if (retVal != UA_STATUSCODE_GOOD) throw open62541::Exceptions::OpcUaErrorException(retVal);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<double> *el) override {
    UA_StatusCode retVal;
    auto myNodeId = generateNodeId(el);
    retVal = UA_Server_writeAccessLevel(SeRoNet::OPCUA::Server::OpcUaServer::instance().getServer(),*myNodeId.NodeId,0);
    if (retVal != UA_STATUSCODE_GOOD) throw open62541::Exceptions::OpcUaErrorException(retVal);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<std::string> *el) override {
    UA_StatusCode retVal;
    auto myNodeId = generateNodeId(el);
    retVal = UA_Server_writeAccessLevel(SeRoNet::OPCUA::Server::OpcUaServer::instance().getServer(),*myNodeId.NodeId,0);
    if (retVal != UA_STATUSCODE_GOOD) throw open62541::Exceptions::OpcUaErrorException(retVal);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementArray *elementArray) override {
    throw SeRoNet::Exceptions::NotImplementedException(__FUNCTION__);
  }

 private:

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
        ownNodeId = open62541::UA_NodeId(SeRoNet::OPCUA::Server::OpcUaServer::instance().getNsIndex1(), ss.str());
    return ownNodeId;
  }

  open62541::UA_NodeId m_parent;
};

namespace SeRoNet {
namespace OPCUA {
namespace Server {

/// @TODO (Sebastian Friedl) add Status Message for Error Handling
PushServerDisabler::PushServerDisabler(
    CommunicationObjects::Description::IVisitableDescription *description,
    const open62541::UA_NodeId &parent) {
  UpdateOpcuaServerVisitor visitor(parent);
  description->accept(&visitor);
}

}
}
}
