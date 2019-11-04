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
#include <utility>
#include "PushServerUpdater.hpp"

#include "../../Exceptions/NotImplementedException.hpp"
#include "../../CommunicationObjects/Description/ComplexType.hpp"
#include "../../CommunicationObjects/Description/ElementPrimitives.hpp"
#include "OpcuaServer.hpp"
#include "../Converter/CommObjArrayToValue.hpp"

/// Internal Class
class UpdateOpcuaServerVisitor :
    public ::SeRoNet::CommunicationObjects::Description::IVisitorDescription {
 public:

  explicit UpdateOpcuaServerVisitor(std::shared_ptr<SeRoNet::OPCUA::Server::OpcUaServer> pOpcUaServer, const OPEN_65241_CPP_NAMESPACE::UA_NodeId &parent)
      : m_parent(parent), m_pOpcUaServer(std::move(pOpcUaServer)) {}

  void visit(SeRoNet::CommunicationObjects::Description::ComplexType *complexObject) override {
    open62541::UA_NodeId ownNodeId = generateNodeId(complexObject);

    UpdateOpcuaServerVisitor visitor(m_pOpcUaServer, ownNodeId);
    for (auto &el: *complexObject) {
      el->accept(&visitor);
    }
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<bool> *el) override {
    UA_StatusCode retVal;

    auto myNodeId = generateNodeId(el);
    auto myVariant = OPEN_65241_CPP_NAMESPACE::UA_Variant();
    auto value = el->get();

    retVal = UA_Variant_setScalarCopy(myVariant.Variant, &value, &UA_TYPES[UA_TYPES_BOOLEAN]);
    if (retVal != UA_STATUSCODE_GOOD) throw OPEN_65241_CPP_NAMESPACE::Exceptions::OpcUaErrorException(retVal);

    retVal = UA_Server_writeValue(m_pOpcUaServer->getServer(),
                                  *myNodeId.NodeId,
                                  *myVariant.Variant);
    if (retVal != UA_STATUSCODE_GOOD) throw OPEN_65241_CPP_NAMESPACE::Exceptions::OpcUaErrorException(retVal);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<int8_t> *el) override {
    UA_StatusCode retVal;

    auto myNodeId = generateNodeId(el);
    auto myVariant = OPEN_65241_CPP_NAMESPACE::UA_Variant();
    auto value = el->get();

    retVal = UA_Variant_setScalarCopy(myVariant.Variant, &value, &UA_TYPES[UA_TYPES_BYTE]);
    if (retVal != UA_STATUSCODE_GOOD) throw OPEN_65241_CPP_NAMESPACE::Exceptions::OpcUaErrorException(retVal);

    retVal = UA_Server_writeValue(m_pOpcUaServer->getServer(),
                                  *myNodeId.NodeId,
                                  *myVariant.Variant);
    if (retVal != UA_STATUSCODE_GOOD) throw OPEN_65241_CPP_NAMESPACE::Exceptions::OpcUaErrorException(retVal);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<int16_t> *el) override {
    UA_StatusCode retVal;

    auto myNodeId = generateNodeId(el);
    auto myVariant = OPEN_65241_CPP_NAMESPACE::UA_Variant();
    auto value = el->get();

    retVal = UA_Variant_setScalarCopy(myVariant.Variant, &value, &UA_TYPES[UA_TYPES_INT16]);
    if (retVal != UA_STATUSCODE_GOOD) throw OPEN_65241_CPP_NAMESPACE::Exceptions::OpcUaErrorException(retVal);

    retVal = UA_Server_writeValue(m_pOpcUaServer->getServer(),
                                  *myNodeId.NodeId,
                                  *myVariant.Variant);
    if (retVal != UA_STATUSCODE_GOOD) throw OPEN_65241_CPP_NAMESPACE::Exceptions::OpcUaErrorException(retVal);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<std::int32_t> *el) override {
    UA_StatusCode retVal;

    auto myNodeId = generateNodeId(el);
    auto myVariant = OPEN_65241_CPP_NAMESPACE::UA_Variant();
    auto value = el->get();

    retVal = UA_Variant_setScalarCopy(myVariant.Variant, &value, &UA_TYPES[UA_TYPES_INT32]);
    if (retVal != UA_STATUSCODE_GOOD) throw OPEN_65241_CPP_NAMESPACE::Exceptions::OpcUaErrorException(retVal);

    retVal = UA_Server_writeValue(m_pOpcUaServer->getServer(),
                                  *myNodeId.NodeId,
                                  *myVariant.Variant);
    if (retVal != UA_STATUSCODE_GOOD) throw OPEN_65241_CPP_NAMESPACE::Exceptions::OpcUaErrorException(retVal);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<int64_t> *el) override {
    UA_StatusCode retVal;

    auto myNodeId = generateNodeId(el);
    auto myVariant = OPEN_65241_CPP_NAMESPACE::UA_Variant();
    auto value = el->get();

    retVal = UA_Variant_setScalarCopy(myVariant.Variant, &value, &UA_TYPES[UA_TYPES_INT64]);
    if (retVal != UA_STATUSCODE_GOOD) throw OPEN_65241_CPP_NAMESPACE::Exceptions::OpcUaErrorException(retVal);

    retVal = UA_Server_writeValue(m_pOpcUaServer->getServer(),
                                  *myNodeId.NodeId,
                                  *myVariant.Variant);
    if (retVal != UA_STATUSCODE_GOOD) throw OPEN_65241_CPP_NAMESPACE::Exceptions::OpcUaErrorException(retVal);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<uint8_t> *el) override {
    UA_StatusCode retVal;

    auto myNodeId = generateNodeId(el);
    auto myVariant = OPEN_65241_CPP_NAMESPACE::UA_Variant();
    auto value = el->get();

    retVal = UA_Variant_setScalarCopy(myVariant.Variant, &value, &UA_TYPES[UA_TYPES_BYTE]);
    if (retVal != UA_STATUSCODE_GOOD) throw OPEN_65241_CPP_NAMESPACE::Exceptions::OpcUaErrorException(retVal);

    retVal = UA_Server_writeValue(m_pOpcUaServer->getServer(),
                                  *myNodeId.NodeId,
                                  *myVariant.Variant);
    if (retVal != UA_STATUSCODE_GOOD) throw OPEN_65241_CPP_NAMESPACE::Exceptions::OpcUaErrorException(retVal);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<uint16_t> *el) override {
    UA_StatusCode retVal;

    auto myNodeId = generateNodeId(el);
    auto myVariant = OPEN_65241_CPP_NAMESPACE::UA_Variant();
    auto value = el->get();

    retVal = UA_Variant_setScalarCopy(myVariant.Variant, &value, &UA_TYPES[UA_TYPES_UINT16]);
    if (retVal != UA_STATUSCODE_GOOD) throw OPEN_65241_CPP_NAMESPACE::Exceptions::OpcUaErrorException(retVal);

    retVal = UA_Server_writeValue(m_pOpcUaServer->getServer(),
                                  *myNodeId.NodeId,
                                  *myVariant.Variant);
    if (retVal != UA_STATUSCODE_GOOD) throw OPEN_65241_CPP_NAMESPACE::Exceptions::OpcUaErrorException(retVal);
  }
  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<uint32_t> *el) override {
    UA_StatusCode retVal;

    auto myNodeId = generateNodeId(el);
    auto myVariant = OPEN_65241_CPP_NAMESPACE::UA_Variant();
    auto value = el->get();

    retVal = UA_Variant_setScalarCopy(myVariant.Variant, &value, &UA_TYPES[UA_TYPES_UINT32]);
    if (retVal != UA_STATUSCODE_GOOD) throw OPEN_65241_CPP_NAMESPACE::Exceptions::OpcUaErrorException(retVal);

    retVal = UA_Server_writeValue(m_pOpcUaServer->getServer(),
                                  *myNodeId.NodeId,
                                  *myVariant.Variant);
    if (retVal != UA_STATUSCODE_GOOD) throw OPEN_65241_CPP_NAMESPACE::Exceptions::OpcUaErrorException(retVal);
  }
  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<uint64_t> *el) override {
    UA_StatusCode retVal;

    auto myNodeId = generateNodeId(el);
    auto myVariant = OPEN_65241_CPP_NAMESPACE::UA_Variant();
    auto value = el->get();

    retVal = UA_Variant_setScalarCopy(myVariant.Variant, &value, &UA_TYPES[UA_TYPES_UINT64]);
    if (retVal != UA_STATUSCODE_GOOD) throw OPEN_65241_CPP_NAMESPACE::Exceptions::OpcUaErrorException(retVal);

    retVal = UA_Server_writeValue(m_pOpcUaServer->getServer(),
                                  *myNodeId.NodeId,
                                  *myVariant.Variant);
    if (retVal != UA_STATUSCODE_GOOD) throw OPEN_65241_CPP_NAMESPACE::Exceptions::OpcUaErrorException(retVal);
  }
  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<float> *el) override {
    UA_StatusCode retVal;

    auto myNodeId = generateNodeId(el);
    auto myVariant = OPEN_65241_CPP_NAMESPACE::UA_Variant();
    auto value = el->get();

    retVal = UA_Variant_setScalarCopy(myVariant.Variant, &value, &UA_TYPES[UA_TYPES_FLOAT]);
    if (retVal != UA_STATUSCODE_GOOD) throw OPEN_65241_CPP_NAMESPACE::Exceptions::OpcUaErrorException(retVal);

    retVal = UA_Server_writeValue(m_pOpcUaServer->getServer(),
                                  *myNodeId.NodeId,
                                  *myVariant.Variant);
    if (retVal != UA_STATUSCODE_GOOD) throw OPEN_65241_CPP_NAMESPACE::Exceptions::OpcUaErrorException(retVal);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<double> *el) override {
    UA_StatusCode retVal;

    auto myNodeId = generateNodeId(el);
    auto myVariant = OPEN_65241_CPP_NAMESPACE::UA_Variant();
    auto value = el->get();

    retVal = UA_Variant_setScalarCopy(myVariant.Variant, &value, &UA_TYPES[UA_TYPES_DOUBLE]);
    if (retVal != UA_STATUSCODE_GOOD) throw OPEN_65241_CPP_NAMESPACE::Exceptions::OpcUaErrorException(retVal);

    retVal = UA_Server_writeValue(m_pOpcUaServer->getServer(),
                                  *myNodeId.NodeId,
                                  *myVariant.Variant);
    if (retVal != UA_STATUSCODE_GOOD) throw OPEN_65241_CPP_NAMESPACE::Exceptions::OpcUaErrorException(retVal);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<std::string> *el) override {
    UA_StatusCode retVal;

    auto myNodeId = generateNodeId(el);
    auto myVariant = OPEN_65241_CPP_NAMESPACE::UA_Variant();
    auto value = el->get();
    open62541::UA_String tmp = open62541::UA_String(value);

    retVal = UA_Variant_setScalarCopy(myVariant.Variant, tmp.String, &UA_TYPES[UA_TYPES_STRING]);
    if (retVal != UA_STATUSCODE_GOOD) throw OPEN_65241_CPP_NAMESPACE::Exceptions::OpcUaErrorException(retVal);

    retVal = UA_Server_writeValue(m_pOpcUaServer->getServer(),
                                  *myNodeId.NodeId,
                                  *myVariant.Variant);
    if (retVal != UA_STATUSCODE_GOOD) throw OPEN_65241_CPP_NAMESPACE::Exceptions::OpcUaErrorException(retVal);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementArray *elementArray) override {
    UA_StatusCode retVal;

    auto myNodeId = generateNodeId(elementArray);
    auto myVariant = OPEN_65241_CPP_NAMESPACE::UA_Variant();

    SeRoNet::OPCUA::Converter::CommObjArrayToValue commObj2Array(elementArray);
    auto value = commObj2Array.Value();
    retVal =  UA_Server_writeValue(m_pOpcUaServer->getServer(),
                                        *myNodeId.NodeId,
                                        value);
    if (retVal != UA_STATUSCODE_GOOD) throw OPEN_65241_CPP_NAMESPACE::Exceptions::OpcUaErrorException(retVal);
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
        ownNodeId = open62541::UA_NodeId(m_pOpcUaServer->getNsIndex1(), ss.str());
    return ownNodeId;
  }

  std::shared_ptr<SeRoNet::OPCUA::Server::OpcUaServer> m_pOpcUaServer;
  OPEN_65241_CPP_NAMESPACE::UA_NodeId m_parent;
};

namespace SeRoNet {
namespace OPCUA {
namespace Server {

/// @TODO (Sebastian Friedl) add Status Message for Error Handling
PushServerUpdater::PushServerUpdater(
    std::shared_ptr<SeRoNet::OPCUA::Server::OpcUaServer> pOpcUaServer,
    CommunicationObjects::Description::IVisitableDescription *description,
    const OPEN_65241_CPP_NAMESPACE::UA_NodeId &parent) {
  UpdateOpcuaServerVisitor visitor(std::move(pOpcUaServer), parent);
  description->accept(&visitor);
}

}
}
}
