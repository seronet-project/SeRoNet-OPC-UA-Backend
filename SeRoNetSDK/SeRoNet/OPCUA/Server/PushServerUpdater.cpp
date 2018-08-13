///
/// \file PushServerUpdater.cpp
/// \author Sebastian Friedl
/// \date 27.02.2018
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

#include "PushServerUpdater.hpp"

#include "../../Exceptions/NotImplementedException.hpp"
#include "../../CommunicationObjects/Description/ComplexType.hpp"
#include "../../CommunicationObjects/Description/ElementPrimitive.hpp"
//#include "../../CommunicationObjects/Description/IVisitableDescription.hpp"
//#include "../../Exceptions/NotImplementedException.hpp"

/// Internal Class
class UpdateOpcuaServerVisitor :
    public ::SeRoNet::CommunicationObjects::Description::IVisitorDescription {
 public:

  UpdateOpcuaServerVisitor(UA_Server *pServer, const OPEN_65241_CPP_NAMESPACE::UA_NodeId &parent, UA_UInt16 nsIndex)
      : m_pServer(pServer),
        m_parent(parent),
        m_nsIndex(nsIndex) {}

  void visit(SeRoNet::CommunicationObjects::Description::ComplexType *complexObject) override {
    open62541::UA_NodeId ownNodeId = generateNodeId(complexObject);

    UpdateOpcuaServerVisitor visitor(m_pServer, ownNodeId, m_nsIndex);
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

    retVal = UA_Server_writeValue(m_pServer, *myNodeId.NodeId, *myVariant.Variant);
    if (retVal != UA_STATUSCODE_GOOD) throw OPEN_65241_CPP_NAMESPACE::Exceptions::OpcUaErrorException(retVal);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<int32_t> *el) override {
    UA_StatusCode retVal;

    auto myNodeId = generateNodeId(el);
    auto myVariant = OPEN_65241_CPP_NAMESPACE::UA_Variant();
    auto value = el->get();

    retVal = UA_Variant_setScalarCopy(myVariant.Variant, &value, &UA_TYPES[UA_TYPES_INT32]);
    if (retVal != UA_STATUSCODE_GOOD) throw OPEN_65241_CPP_NAMESPACE::Exceptions::OpcUaErrorException(retVal);

    retVal = UA_Server_writeValue(m_pServer, *myNodeId.NodeId, *myVariant.Variant);
    if (retVal != UA_STATUSCODE_GOOD) throw OPEN_65241_CPP_NAMESPACE::Exceptions::OpcUaErrorException(retVal);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<float> *el) override {
    UA_StatusCode retVal;

    auto myNodeId = generateNodeId(el);
    auto myVariant = OPEN_65241_CPP_NAMESPACE::UA_Variant();
    auto value = el->get();

    retVal = UA_Variant_setScalarCopy(myVariant.Variant, &value, &UA_TYPES[UA_TYPES_FLOAT]);
    if (retVal != UA_STATUSCODE_GOOD) throw OPEN_65241_CPP_NAMESPACE::Exceptions::OpcUaErrorException(retVal);

    retVal = UA_Server_writeValue(m_pServer, *myNodeId.NodeId, *myVariant.Variant);
    if (retVal != UA_STATUSCODE_GOOD) throw OPEN_65241_CPP_NAMESPACE::Exceptions::OpcUaErrorException(retVal);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<double> *el) override {
    UA_StatusCode retVal;

    auto myNodeId = generateNodeId(el);
    auto myVariant = OPEN_65241_CPP_NAMESPACE::UA_Variant();
    auto value = el->get();

    retVal = UA_Variant_setScalarCopy(myVariant.Variant, &value, &UA_TYPES[UA_TYPES_DOUBLE]);
    if (retVal != UA_STATUSCODE_GOOD) throw OPEN_65241_CPP_NAMESPACE::Exceptions::OpcUaErrorException(retVal);

    retVal = UA_Server_writeValue(m_pServer, *myNodeId.NodeId, *myVariant.Variant);
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

    retVal = UA_Server_writeValue(m_pServer, *myNodeId.NodeId, *myVariant.Variant);
    if (retVal != UA_STATUSCODE_GOOD) throw OPEN_65241_CPP_NAMESPACE::Exceptions::OpcUaErrorException(retVal);
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
    open62541::UA_NodeId ownNodeId = open62541::UA_NodeId(1, ss.str());// FIXME use generic Namespace
    return ownNodeId;
  }

  const UA_UInt16 ns0 = 0;
  const UA_UInt16 m_nsIndex;
  UA_Server *m_pServer;
  OPEN_65241_CPP_NAMESPACE::UA_NodeId m_parent;
};

namespace SeRoNet {
namespace OPCUA {
namespace Server {

/// @TODO (Sebastian Friedl) add Status Message for Error Handling
PushServerUpdater::PushServerUpdater(
    CommunicationObjects::Description::IVisitableDescription *description,
    UA_Server *pServer,
    const OPEN_65241_CPP_NAMESPACE::UA_NodeId &parent,
    UA_UInt16 nsIndex) : m_pServer(pServer), m_nsIndex(nsIndex) {
  UpdateOpcuaServerVisitor visitor(pServer, parent, nsIndex);
  description->accept(&visitor);
}

}
}
}
