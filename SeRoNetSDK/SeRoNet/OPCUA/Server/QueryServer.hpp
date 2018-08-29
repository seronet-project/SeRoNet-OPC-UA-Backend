/**
* @author Sebastian Friedl
*/
#pragma once

#include <iostream>
#include <csignal>
#include <mutex>
#include <map>
#include <thread>
#include <string>
#include <utility>

#include <open62541.h>

#include <Open62541Cpp/UA_Argument.hpp>
#include <Open62541Cpp/UA_ArrayOfArgument.hpp>
#include <Open62541Cpp/UA_ArrayOfVariant.hpp>
#include <sstream>
#include <Open62541Cpp/UA_NodeId.hpp>

#include "../../../../SmartSoftComponentDeveloperAPIcpp/SmartSoft_CD_API/smartIComponent.h"
#include "../../../../SmartSoftComponentDeveloperAPIcpp/SmartSoft_CD_API/smartStatusCode.h"
#include "../../../../SmartSoftComponentDeveloperAPIcpp/SmartSoft_CD_API/smartICommunicationObject.h"
#include "../../../../SmartSoftComponentDeveloperAPIcpp/SmartSoft_CD_API/smartIQueryServerPattern_T.h"

#include "../../Utils/Component.hpp"
#include "../../CommunicationObjects/ICommunicationObject.hpp"
#include "../Client/Converter/CommObjectToUaVariantArray.hpp"
#include "../Client/Converter/UaVariantArrayToCommObject.hpp"
#include "CommObjectToUaArgument.hpp"
#include "QueryServerHandler.hpp"
#include "OpcuaServer.hpp"
#include "../../Exceptions/NotImplementedException.hpp"
#include "../../CommunicationObjects/Description/SelfDescription.hpp"

namespace SeRoNet {
namespace OPCUA {
namespace Server {

template<typename T_REQUEST, typename T_ANSWER>
class QueryServer :
    public Smart::IQueryServerPattern<T_REQUEST, T_ANSWER, int> {
  // TODO(sebastian) change int to SmartID implementation
 private:
  /// management class of the component
  Smart::IComponent *m_component;
  /// name of service
  std::string m_service;

  std::map<int, T_ANSWER> m_answers;

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
  QueryServer(Smart::IComponent *component, const std::string &service);

  /** Destructor.
  *  Properly disconnects all service requestors in case of destruction
  *  such that all pending queries are handled correctly at client side
  *  even when the service provider disappears during pending queries.
  */
  virtual ~QueryServer() noexcept = default;

  /** Provide answer to be sent back to the requestor.
  *
  *  Member function is thread safe and thread reentrant.
  *  TODO (sebastian) change int to smartsoft id
  *
  *  @param id identifies the request to which the answer belongs
  *  @param answer is the reply itself.
  *
  *  @return status code:
      */
  virtual Smart::StatusCode answer(const int &id, const T_ANSWER &answer);

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

  OPCUA::Client::Converter::UaVariantArrayToCommObject
      conv(open62541::UA_ArrayOfVariant(input, inputSize),
           CommunicationObjects::Description::SelfDescription(&request, "").get());

  int id = rand();  // TODO(Friedl) ersetzten mit std:future
  Smart::QueryServerInputType<T_REQUEST, int> fullRequest = {request, id};
  friendThis->notify_input(fullRequest);
  while (friendThis->m_answers.find(id) == friendThis->m_answers.end()) {
    std::this_thread::yield();
  }
  auto tmp = static_cast<open62541::UA_ArrayOfVariant> (Client::Converter::CommObjectToUaVariantArray
      (CommunicationObjects::Description::SelfDescription(&friendThis->m_answers.at(id), "").get()));

  ///\todo use UA_Variant_setArrayCopy()!
  for (int i = 0; i < tmp.VariantsSize; i++) {
    UA_copy(&tmp.Variants[i], &output[i], &UA_TYPES[UA_TYPES_VARIANT]);
  }
  friendThis->m_answers.erase(id);
  return UA_STATUSCODE_GOOD;
}
#pragma clang diagnostic pop

template<typename T_REQUEST, typename T_ANSWER>
inline QueryServer<T_REQUEST, T_ANSWER>::QueryServer(
    Smart::IComponent *component,
    const std::string &service
) :
    Smart::IQueryServerPattern<T_REQUEST, T_ANSWER, int>::IQueryServerPattern(component, service),
    m_component(component),
    m_service(service) {
  SeRoNet::OPCUA::Server::OpcUaServer::instance().initServer(component->getName());

  T_REQUEST *inputCommObject = new T_REQUEST;
  OPEN_65241_CPP_NAMESPACE::UA_ArrayOfArgument
      inputArguments =
      CommObjectToUaArgumentArray(CommunicationObjects::Description::SelfDescription(inputCommObject, "input").get());
  T_ANSWER *outputCommObject = new T_ANSWER;
  OPEN_65241_CPP_NAMESPACE::UA_ArrayOfArgument
      outputArguments =
      CommObjectToUaArgumentArray(CommunicationObjects::Description::SelfDescription(outputCommObject, "output").get());

  UA_MethodAttributes helloAttr;
  UA_MethodAttributes_init(&helloAttr);
  helloAttr.description = UA_LOCALIZEDTEXT_ALLOC("en_US", "Component.Query");
  helloAttr.displayName = UA_LOCALIZEDTEXT_ALLOC("en_US", this->m_service.c_str());
  helloAttr.executable = true;
  helloAttr.userExecutable = true;

  std::stringstream ss;
  ss << "85." << this->m_service.c_str(); //TODO use a own function to create nodeIds

  UA_Server_addMethodNode(
      OpcUaServer::instance().getServer(),
      UA_NODEID_STRING_ALLOC(OpcUaServer::instance().getNsIndex1(), ss.str().c_str()),
      UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
      UA_NODEID_NUMERIC(0, UA_NS0ID_HASORDEREDCOMPONENT),
      UA_QUALIFIEDNAME_ALLOC(1, this->m_service.c_str()),
      helloAttr,
      &methodCallback,
      inputArguments.arraySize,
      inputArguments.arguments,
      outputArguments.arraySize,
      outputArguments.arguments,
      this,
      NULL);
}

template<typename T_REQUEST, typename T_ANSWER>
Smart::StatusCode QueryServer<T_REQUEST, T_ANSWER>::answer(const int &id, const T_ANSWER &answer) {
  std::pair<int, T_ANSWER> answer_pair(id, answer);
  m_answers.insert(answer_pair);
  return Smart::StatusCode::SMART_OK;
}

}  // namespace Server
}  // namespace OPCUA
}  // namespace SeRoNet
