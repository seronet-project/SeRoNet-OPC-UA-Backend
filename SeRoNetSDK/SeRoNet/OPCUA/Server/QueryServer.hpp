/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#pragma once

#include <iostream>
#include <csignal>
#include <mutex>
#include <map>
#include <thread>
#include <string>
#include <utility>

#include <open62541/server.h>

#include <Open62541Cpp/UA_Argument.hpp>
#include <Open62541Cpp/UA_ArrayOfArgument.hpp>
#include <Open62541Cpp/UA_ArrayOfVariant.hpp>
#include <sstream>
#include <Open62541Cpp/UA_NodeId.hpp>

#include <smartIComponent.h>
#include <smartStatusCode.h>
#include <smartICommunicationObject.h>
#include <smartIQueryServerPattern_T.h>
#include <smartNumericCorrelationId.h>

#include "../../Utils/Component.hpp"
#include "../../CommunicationObjects/ICommunicationObject.hpp"
#include "../Converter/CommObjectToUaVariantArray.hpp"
#include "../Converter/UaVariantArrayToCommObject.hpp"
#include "../Converter/CommObjectToUaArgument.hpp"
#include "QueryServerHandler.hpp"
#include "OpcuaServer.hpp"
#include "../../Exceptions/NotImplementedException.hpp"
#include "../../CommunicationObjects/Description/SelfDescription.hpp"

namespace SeRoNet {
namespace OPCUA {
namespace Server {

template<typename T_REQUEST, typename T_ANSWER>
class QueryServer :
    public Smart::IQueryServerPattern<T_REQUEST, T_ANSWER> {
 private:
  /// management class of the component
  Utils::Component *m_component;
  /// name of service
  std::string m_service;
  open62541Cpp::UA_NodeId m_methodNodeId;

  std::map<Smart::NumericCorrelationId, T_ANSWER> m_answers;

  static UA_StatusCode methodCallback(
      UA_Server *server,
      const UA_NodeId *sessionId,
      void *sessionContext,
      const UA_NodeId *methodId,
      void *methodContext,
      const UA_NodeId *objectId,
      void *objectContext,
      size_t inputSize,
      const UA_Variant *input,
      size_t outputSize,
      UA_Variant *output);

 public:
  using IQueryServerBase = Smart::IQueryServerPattern<T_REQUEST,T_ANSWER>;
  using typename IQueryServerBase::IQueryServerHandlerPtr;

  /** Constructor.
  *
  *  Note that a handler has to be supplied. Without a handler, the
  *  QueryServer could not accept a query.
  *
  *  (Currently exception not thrown)
  *
  *  @param component management class of the component
  *  @param service   name of the service
  */
  QueryServer(Utils::Component *component, const std::string &service, IQueryServerHandlerPtr query_handler = nullptr);

  /** Destructor.
  *  Properly disconnects all service requestors in case of destruction
  *  such that all pending queries are handled correctly at client side
  *  even when the service provider disappears during pending queries.
  */
  virtual ~QueryServer();

  /** Provide answer to be sent back to the requestor.
  *
  *  Member function is thread safe and thread reentrant.
  *
  *  @param id identifies the request to which the answer belongs
  *  @param answer is the reply itself.
  *
  *  @return status code:
      */
  virtual Smart::StatusCode answer(const Smart::QueryIdPtr id, const T_ANSWER &answer);

  virtual void serverInitiatedDisconnect() {
    throw SeRoNet::Exceptions::NotImplementedException(__FUNCTION__);
  }
};

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
template<typename T_REQUEST, typename T_ANSWER>
UA_StatusCode QueryServer<T_REQUEST, T_ANSWER>::methodCallback(
    UA_Server *server,
    const UA_NodeId *sessionId,
    void *sessionContext,
    const UA_NodeId *methodId,
    void *methodContext,
    const UA_NodeId *objectId,
    void *objectContext,
    size_t inputSize,
    const UA_Variant *input,
    size_t outputSize,
    UA_Variant *output) {
  QueryServer<T_REQUEST, T_ANSWER> *friendThis = static_cast<QueryServer<T_REQUEST, T_ANSWER> *>(methodContext);

  T_REQUEST request;

  OPCUA::Converter::UaVariantArrayToCommObject
      conv(open62541Cpp::UA_ArrayOfVariant(input, inputSize),
           CommunicationObjects::Description::SelfDescription(&request, "").get());

  auto id_ptr = std::make_shared<Smart::NumericCorrelationId>(rand()); // TODO(Friedl) ersetzten mit std:future
  friendThis->handleQuery(id_ptr, request);
  while (friendThis->m_answers.find(*id_ptr) == friendThis->m_answers.end()) {
    std::this_thread::yield();
  }
  auto tmp = static_cast<open62541Cpp::UA_ArrayOfVariant> (Converter::CommObjectToUaVariantArray
      (CommunicationObjects::Description::SelfDescription(&friendThis->m_answers.at(*id_ptr), "").get()));

  ///\todo use UA_Variant_setArrayCopy()!
  for (int i = 0; i <tmp.VariantsSize; i++) {
    UA_copy(&tmp.Variants[i], &output[i], &UA_TYPES[UA_TYPES_VARIANT]);
  }
  friendThis->m_answers.erase(*id_ptr);
  return UA_STATUSCODE_GOOD;
}
#pragma clang diagnostic pop

template<typename T_REQUEST, typename T_ANSWER>
inline QueryServer<T_REQUEST, T_ANSWER>::QueryServer(
    Utils::Component *component,
    const std::string &service,
    IQueryServerHandlerPtr query_handler
) :
    IQueryServerBase(component, service, query_handler),
    m_component(component),
    m_service(service) {

  T_REQUEST inputCommObject;
  open62541Cpp::UA_ArrayOfArgument
      inputArguments =
      Converter::CommObjectToUaArgumentArray(CommunicationObjects::Description::SelfDescription(&inputCommObject, "input").get());
  T_ANSWER outputCommObject;
  open62541Cpp::UA_ArrayOfArgument
      outputArguments =
      Converter::CommObjectToUaArgumentArray(CommunicationObjects::Description::SelfDescription(&outputCommObject, "output").get());

  UA_MethodAttributes helloAttr;
  UA_MethodAttributes_init(&helloAttr);
  helloAttr.description = UA_LOCALIZEDTEXT_ALLOC("en_US", "Component.Query");
  helloAttr.displayName = UA_LOCALIZEDTEXT_ALLOC("en_US", this->m_service.c_str());
  helloAttr.executable = true;
  helloAttr.userExecutable = true;

  std::stringstream ss;
  ss <<"85." <<this->m_service.c_str(); //TODO use a own function to create nodeIds

  UA_QualifiedName qualifiedName = UA_QUALIFIEDNAME_ALLOC(1, this->m_service.c_str());
  open62541Cpp::UA_NodeId newNodeId(m_component->getOpcUaServer()->getNsIndex1(), ss.str().c_str());

  UA_Server_addMethodNode(
      m_component->getOpcUaServer()->getServer(),
      *newNodeId.NodeId,
      UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
      UA_NODEID_NUMERIC(0, UA_NS0ID_HASORDEREDCOMPONENT),
      qualifiedName,
      helloAttr,
      &methodCallback,
      inputArguments.arraySize,
      inputArguments.arguments,
      outputArguments.arraySize,
      outputArguments.arguments,
      this,
      m_methodNodeId.NodeId);

  UA_QualifiedName_clear(&qualifiedName);
  UA_MethodAttributes_clear(&helloAttr);
}

template<typename T_REQUEST, typename T_ANSWER>
Smart::StatusCode QueryServer<T_REQUEST, T_ANSWER>::answer(const Smart::QueryIdPtr id, const T_ANSWER &answer) {
  auto numeric_id_ptr = std::dynamic_pointer_cast<Smart::NumericCorrelationId>(id);
  if(!numeric_id_ptr) {
    return Smart::StatusCode::SMART_WRONGID;
  }
  std::pair<Smart::NumericCorrelationId, T_ANSWER> answer_pair(*numeric_id_ptr, answer);
  m_answers.insert(answer_pair);
  return Smart::StatusCode::SMART_OK;
}

template<typename T_REQUEST, typename T_ANSWER>
QueryServer<T_REQUEST, T_ANSWER>::~QueryServer() {
  UA_Server *pServer = m_component->getOpcUaServer()->getServer();
  if(!UA_NodeId_isNull(m_methodNodeId.NodeId) && pServer)
  {
    UA_Server_deleteNode(pServer, *m_methodNodeId.NodeId, UA_TRUE);
    m_methodNodeId = UA_NODEID_NULL;
  }
}

}  // namespace Server
}  // namespace OPCUA
}  // namespace SeRoNet
