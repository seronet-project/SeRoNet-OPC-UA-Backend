///
/// \file CommObjectToPushModell.cpp
/// \author Sebastian Friedl
/// \date 26.02.2018
///
#include <list>
#include <sstream>

#include <Open62541Cpp/UA_Argument.hpp>
#include <Open62541Cpp/UA_String.hpp>
#include <Open62541Cpp/UA_NodeId.hpp>
#include <Open62541Cpp/Exceptions/OpcUaErrorException.hpp>
#include <open62541/open62541.h>

//#include <open62541.h>
//#include <open62541/open62541.h>
#include "CommObjectToPushModell.hpp"
#include "../../Exceptions/NotImplementedException.hpp"
#include "../../CommunicationObjects/Description/ComplexType.hpp"
#include "../../CommunicationObjects/Description/ElementPrimitive.hpp"
//#include "../../CommunicationObjects/Description/IVisitableDescription.hpp"
//#include "../../Exceptions/NotImplementedException.hpp"

/// Internal Class
class ToPushModellVisitor :
    public ::SeRoNet::CommunicationObjects::Description::IVisitorDescription {
 public:

  ToPushModellVisitor(UA_Server *pServer, const OPEN_65241_CPP_NAMESPACE::UA_NodeId &parent, UA_UInt16 nsIndex)
      : m_pServer(pServer),
        m_parent(parent),
        m_nsIndex(nsIndex) {}

  /// WARNING this Methode is untested
  void visit(SeRoNet::CommunicationObjects::Description::ComplexType *complexObject) override {
    const open62541::UA_NodeId ownNodeId = generateNodeId(complexObject);
    UA_ObjectAttributes attr = UA_ObjectAttributes_default;
    attr.displayName = UA_LOCALIZEDTEXT_ALLOC("", complexObject->getName().c_str());

    UA_StatusCode retVal = UA_Server_addObjectNode(
        m_pServer,
        *ownNodeId.NodeId,
        *m_parent.NodeId,
        UA_NODEID_NUMERIC(ns0, UA_NS0ID_ORGANIZES),
        UA_QUALIFIEDNAME_ALLOC(m_nsIndex, complexObject->getName().c_str()),
        UA_NODEID_NUMERIC(ns0, UA_NS0ID_BASEOBJECTTYPE),
        attr, nullptr, nullptr);

    if (retVal != UA_STATUSCODE_GOOD) throw OPEN_65241_CPP_NAMESPACE::Exceptions::OpcUaErrorException(retVal);


    ToPushModellVisitor visitor(m_pServer, ownNodeId, m_nsIndex);
    for (auto &el: *complexObject) {
      el->accept(&visitor);
    }
  }

  void createPrimitive(SeRoNet::CommunicationObjects::Description::IDescription *el, std::size_t uaTypesIndex)
  {
    UA_StatusCode retVal;
    UA_VariableAttributes attr = UA_VariableAttributes_default;

    attr.userAccessLevel = UA_ACCESSLEVELMASK_READ;
    attr.accessLevel = UA_ACCESSLEVELMASK_READ;
    UA_NodeId_copy(&UA_TYPES[uaTypesIndex].typeId, &attr.dataType);
    void *dataContents = UA_malloc(UA_TYPES[uaTypesIndex].memSize);
    UA_init(dataContents, &UA_TYPES[uaTypesIndex]);
    UA_Variant_setScalar(&attr.value, dataContents, &UA_TYPES[uaTypesIndex]);
    attr.displayName = UA_LOCALIZEDTEXT_ALLOC("", el->getName().c_str());

    auto myNodeId = generateNodeId(el);

    retVal = UA_Server_addVariableNode(m_pServer,
                                       *myNodeId.NodeId,
                                       *m_parent.NodeId,
                                       UA_NODEID_NUMERIC(ns0, UA_NS0ID_HASCOMPONENT),
                                       UA_QUALIFIEDNAME_ALLOC(m_nsIndex, el->getName().c_str()),
                                       UA_NODEID_NUMERIC(ns0, UA_NS0ID_BASEDATAVARIABLETYPE),
                                       attr,
                                       nullptr, nullptr);
    if (retVal != UA_STATUSCODE_GOOD) throw OPEN_65241_CPP_NAMESPACE::Exceptions::OpcUaErrorException(retVal);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<bool> *el) override {
    createPrimitive(el, UA_TYPES_BOOLEAN);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<int32_t> *el) override {
    createPrimitive(el, UA_TYPES_INT32);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<float> *el) override {
    createPrimitive(el, UA_TYPES_FLOAT);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<double> *el) override {
    createPrimitive(el, UA_TYPES_DOUBLE);
  }


  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<std::string> *el) override {
    createPrimitive(el, UA_TYPES_STRING);
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
    open62541::UA_NodeId ownNodeId = open62541::UA_NodeId(m_nsIndex, ss.str());
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

CommObjectToPushModell::CommObjectToPushModell(
    CommunicationObjects::Description::IVisitableDescription *description,
    UA_Server *pServer,
    const OPEN_65241_CPP_NAMESPACE::UA_NodeId &parent,
    UA_UInt16 nsIndex) : m_pServer(pServer), m_nsIndex(nsIndex) {
  ToPushModellVisitor visitor(pServer, parent, nsIndex);
  description->accept(&visitor);
}

}
}
}
