/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#pragma once

#include <smartIEventServerPattern_T.h>
#include <smartStatusCode.h>
#include "../../Utils/Component.hpp"
#include <sstream>
#include <Open62541Cpp/UA_String.hpp>
#include <open62541/server.h>
#include "../../CommunicationObjects/Description/SelfDescription.hpp"
#include "PushServerUpdater.hpp"
#include <smartIEventClientPattern_T.h>
#include "SeRoNet/OPCUA/Converter/CommObjectToUaArgument.hpp"
#include "../../Exceptions/NotImplementedException.hpp"
#include "SeRoNet/OPCUA/Converter/CommObjectToPushModel.hpp"
#include "SeRoNet/OPCUA/Converter/UaVariantArrayToCommObject.hpp"
#include "../../Exceptions/NoDataAvailableException.hpp"
#include "OpcuaServer.hpp"
#include "PushServerDisabler.hpp"
#include "PushServerEnabler.hpp"

namespace SeRoNet {
namespace OPCUA {
namespace Server {

//structure to store the information regarding a single activation of a Event, check for admission
/**
template<class T_ParameterType>
struct EventInfo {
  T_ParameterType param;
  open62541::UA_NodeId nodeId;
  EventInfo() {};
};**/

template<class T_ParameterType, class T_ResultType, class T_StatusType, class T_IdentificationType>
class EventServer :
    public Smart::IEventServerPattern<T_ParameterType, T_ResultType, T_StatusType, T_IdentificationType> {
 protected:

  virtual void serverInitiatedDisconnect() {
    throw SeRoNet::Exceptions::NotImplementedException(__FUNCTION__);
  }

  SeRoNet::Utils::Component *m_pComponent;

  /**method callback that implements the activation of the event by the clients
   *  for each activation the method creates the result node and gives back the node id of this
   *  result node.
   *  The server stores each parameter together with the corresponding node id
   */
  static UA_StatusCode activateCallback(UA_Server *server,
                                        const UA_NodeId *sessionId, void *sessionContext,
                                        const UA_NodeId *methodId, void *methodContext,
                                        const UA_NodeId *objectId, void *objectContext,
                                        size_t inputSize, const UA_Variant *input,
                                        size_t outputSize, UA_Variant *output);

  /** method callback that implements the deactivation of an event that has been previously activated by a client
   *  takes in the node id of the activation that the client wishes to deactivate, removes the node and deletes it from
   *  the map "m_EventInfos"

  static UA_StatusCode deactivateCallback(UA_Server *server,
                                          const UA_NodeId *sessionId, void *sessionContext,
                                          const UA_NodeId *methodId, void *methodContext,
                                          const UA_NodeId *objectId, void *objectContext,
                                          size_t inputSize, const UA_Variant *input,
                                          size_t outputSize, UA_Variant *output);
**/
  //node Id of the object node
  open62541::UA_NodeId m_objNodeId;
 private:
  /// management class of the component
  /// name of service
  std::string m_service;

  //for organizing the activations of the event
  std::map<open62541::UA_NodeId, T_ParameterType> m_EventInfos;
 public:

  /**Constructor
   * creates an Opc Object node derived of the object folder
   * creates a method node as a component of the object
   * the method is used for the activation of events by a client
   * @param component
   * @param serviceName
   * @param eventTestHandler
   */

  EventServer(SeRoNet::Utils::Component *component,
              const std::string &serviceName,
              Smart::IEventTestHandler<T_ParameterType, T_ResultType, T_StatusType> *eventTestHandler);

  /** Destructor.
     *  Properly disconnects all service requestors in case of destruction
     *  such that all pending queries are handled correctly at client side
     *  even when the service provider disappears during pending queries.
     */
  virtual ~EventServer() = default;

  Smart::StatusCode put(const T_StatusType &newState) override;

  bool m_disabled = true;
};
//method callback equates to activation method
template<class T_ParameterType, class T_ResultType, class T_StatusType, class T_IdentificationType>
UA_StatusCode EventServer<T_ParameterType, T_ResultType, T_StatusType, T_IdentificationType>::activateCallback(
    UA_Server *server,
    const UA_NodeId */*sessionId*/,
    void */*sessionContext*/,
    const UA_NodeId */*methodId*/,
    void *methodContext,
    const UA_NodeId *objectId,
    void */*objectContext*/,
    size_t inputSize,
    const UA_Variant *input,
    size_t /*outputSize*/,
    UA_Variant *output) {

  EventServer<T_ParameterType, T_ResultType, T_StatusType, T_IdentificationType> *friendThis =
      static_cast<EventServer<T_ParameterType, T_ResultType, T_StatusType, T_IdentificationType> *>(methodContext);

  /// \todo check for valid obj NodeId!!
  //auto callobjNodeId = open62541::UA_NodeId::FromConstNodeId(objectId);

  //Create the Opc Node

  ///\FIXME \todo Do NOT use pointer?!
  T_ResultType *ResultObj = new T_ResultType;
  //since every Node should get the same Id because they are all of the same type, every node gets a different nsIndex
  //UA_UInt16 nsIndex = rand();

  UA_ObjectAttributes actvAtrr = UA_ObjectAttributes_default;
  actvAtrr.displayName = UA_LOCALIZEDTEXT_ALLOC("en-Us", "Activation");

  //return the value of the assigned Id to store it and identify the activation later
  open62541::UA_NodeId m_NodeId;
  UA_Server_addObjectNode(server,
                          UA_NODEID_NULL,
                          *objectId,
                          UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                          UA_QUALIFIEDNAME_ALLOC(1, "Activation"),
                          UA_NODEID_NUMERIC(0, UA_NS0ID_BASEOBJECTTYPE),
                          actvAtrr, nullptr, m_NodeId.NodeId);

  Converter::CommObjectToPushModel(
      friendThis->m_pComponent->getOpcUaServer(),
      CommunicationObjects::Description::SelfDescription(ResultObj, friendThis->serviceName).get(),
      m_NodeId);

  //store the information and id
  T_ParameterType m_param;

  SeRoNet::OPCUA::Converter::UaVariantArrayToCommObject
      UaVariantArrayToCommObject(open62541::UA_ArrayOfVariant(input, inputSize),
                                 CommunicationObjects::Description::SelfDescription(&m_param, "").get());

  std::pair<open62541::UA_NodeId, T_ParameterType> m_eventPair = std::make_pair(m_NodeId, m_param);
  friendThis->m_EventInfos.insert(m_eventPair);
  std::stringstream ss;
  ss <<m_NodeId.NodeId->identifier.numeric <<"." <<friendThis->serviceName;
  ///\todo determine general namespace index (might not be always 2)
  open62541::UA_NodeId retNodeId(2, ss.str());
  UA_StatusCode retval = UA_Variant_setScalarCopy(output, retNodeId.NodeId, &UA_TYPES[UA_TYPES_NODEID]);
  return retval;
};
/**
//method callback equates to deactivation method
template<class T_ParameterType, class T_ResultType, class T_StatusType, class T_IdentificationType>
UA_StatusCode EventServer<T_ParameterType, T_ResultType, T_StatusType, T_IdentificationType>::deactivateCallback(
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

  EventServer<T_ParameterType, T_ResultType, T_StatusType, T_IdentificationType> *friendThis =
      static_cast<EventServer<T_ParameterType, T_ResultType, T_StatusType, T_IdentificationType> *>(methodContext);

  const open62541::UA_NodeId inputId = *static_cast<open62541::UA_NodeId*>(input->data); //check

  std::map::size_type count = friendThis->m_EventInfos.erase(inputId);
  if (count != 1){
    UA_StatusCode retval = UA_STATUSCODE_BADNOENTRYEXISTS;
  };

  UA_StatusCode retval = UA_Server_deleteNode(server, *inputId.NodeId, true);

  //todo: check for rest of nodes
  return retval;
};**/

//Constructor
template<class T_ParameterType, class T_ResultType, class T_StatusType, class T_IdentificationType>
inline EventServer<T_ParameterType,
                   T_ResultType,
                   T_StatusType,
                   T_IdentificationType>::EventServer(SeRoNet::Utils::Component *component,
                                                      const std::string &serviceName,
                                                      Smart::IEventTestHandler<
                                                          T_ParameterType,
                                                          T_ResultType,
                                                          T_StatusType> *eventTestHandler)
    : Smart::IEventServerPattern<
    T_ParameterType,
    T_ResultType,
    T_StatusType,
    T_IdentificationType>(component, serviceName, eventTestHandler),
      m_service(serviceName),
      m_pComponent(component) {
  UA_Server *server = component->getOpcUaServer()->getServer();
  //UA_NodeId objectsFolderNodeId(component->getNsIndex0(), UA_NS0ID_OBJECTSFOLDER);
  UA_NodeId objectsFolderNodeId = UA_NODEID_NUMERIC(component->getOpcUaServer()->getNsIndex0(), UA_NS0ID_OBJECTSFOLDER);
  /// \fixme determine by URI
  const UA_UInt16 nsIndex = 2;

  //create the object node to manage the result nodes
  UA_ObjectAttributes m_attr = UA_ObjectAttributes_default;
  m_attr.displayName = UA_LOCALIZEDTEXT_ALLOC("", this->m_service.c_str());
  /// \FIXME generate NodeId by nameservice
  m_objNodeId = UA_NODEID_STRING_ALLOC(nsIndex, ("85." + serviceName).c_str());
  UA_Server_addObjectNode(server,
                          *m_objNodeId.NodeId,
                          objectsFolderNodeId,
                          UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
                          UA_QUALIFIEDNAME_ALLOC(1, this->m_service.c_str()),
                          UA_NODEID_NUMERIC(0, UA_NS0ID_BASEOBJECTTYPE), m_attr, nullptr, nullptr);

//add a method node for the activation
  T_ParameterType *inputCommObject = new T_ParameterType;
  //have a communication object as input parameter
  OPEN_65241_CPP_NAMESPACE::UA_ArrayOfArgument
      actInputArguments =
      Converter::CommObjectToUaArgumentArray(CommunicationObjects::Description::SelfDescription(inputCommObject, "input").get());

  //the assigned id of the created node as output parameter
  UA_Argument outputArgument;
  UA_Argument_init(&outputArgument);
  outputArgument.description = UA_LOCALIZEDTEXT_ALLOC("en-US", "The assigned id");
  outputArgument.name = UA_STRING_ALLOC("The assigned id");
  outputArgument.dataType = UA_TYPES[UA_TYPES_NODEID].typeId;
  outputArgument.valueRank = -2;

  UA_MethodAttributes activateAttr = UA_MethodAttributes_default;
  activateAttr.description = UA_LOCALIZEDTEXT_ALLOC("en-US", "Activates an event by a Client");
  activateAttr.displayName = UA_LOCALIZEDTEXT_ALLOC("en-US", "Activation method");
  activateAttr.executable = true;
  activateAttr.userExecutable = true;
  UA_Server_addMethodNode(server,
                          UA_NODEID_STRING_ALLOC(nsIndex, "Activation method"),
                          *m_objNodeId.NodeId,
                          UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                          UA_QUALIFIEDNAME_ALLOC(nsIndex, "Event activation Method"),
                          activateAttr,
                          &activateCallback, actInputArguments.arraySize,
                          actInputArguments.arguments, 1,
                          &outputArgument, this, nullptr);
  /**
  //add a method node for deactivation
  //take the id of the activation as input parameter
  UA_Argument deactInputArgument;
  UA_Argument_init(&deactInputArgument);
  deactInputArgument.description = UA_LOCALIZEDTEXT("en-US", "The Id of the activation to be removed");
  deactInputArgument.name = UA_STRING("Node Id");
  deactInputArgument.dataType = UA_TYPES[UA_TYPES_NODEID].typeId;
  deactInputArgument.valueRank = -2;

  //have no output parameter
  UA_MethodAttributes deactivateAttr = UA_MethodAttributes_default;
  deactivateAttr.description = UA_LOCALIZEDTEXT("en-US", "Deactivates an event by a client");
  deactivateAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Deactivation method");
  deactivateAttr.executable = true;
  deactivateAttr.userExecutable = true;
  UA_Server_addMethodNode(server,
                          UA_NODEID_STRING(1, "Deactivation method"),
                          *m_objNodeId.NodeId,
                          UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                          UA_QUALIFIEDNAME_ALLOC(1, "Event deactivation method"),
                          deactivateAttr,
                          &deactivateCallback,
                          1,
                          &deactInputArgument,
                          nullptr,
                          nullptr, nullptr, nullptr);
**/
};

template<class T_ParameterType, class T_ResultType, class T_StatusType, class T_IdentificationType>
Smart::StatusCode EventServer<T_ParameterType, T_ResultType, T_StatusType, T_IdentificationType>::
put(const T_StatusType &newState) {

  if (!m_EventInfos.empty()) {
    for (auto it = m_EventInfos.begin(); it != m_EventInfos.end(); ++it) {
      UA_Server *server = m_pComponent->getOpcUaServer()->getServer();
      T_ResultType result;
      T_StatusType s(newState);
      open62541::UA_NodeId m_NodeId = it->first;
      T_ParameterType m_param = it->second;

      if (this->testHandler->testEvent(m_param, result, s)) {
        if (m_disabled) {
          PushServerEnabler(m_pComponent->getOpcUaServer(), CommunicationObjects::Description::SelfDescription(&s, this->serviceName).get(), m_NodeId);
          m_disabled = false;
        }

        //write the new status into all result nodes
        PushServerUpdater(m_pComponent->getOpcUaServer(), CommunicationObjects::Description::SelfDescription(&s, this->serviceName).get(), m_NodeId);
      } else {
        if (!m_disabled) {
          PushServerDisabler(m_pComponent->getOpcUaServer(), CommunicationObjects::Description::SelfDescription(&s, this->serviceName).get(), m_NodeId);
          m_disabled = true;
        }
      }

    }
    return Smart::StatusCode::SMART_OK;
  }
  return Smart::StatusCode::SMART_NODATA;
}

} // namespace Client
} // namespace OPCUA
} // namespace SeRoNet

/*Todo: -get event method. either here or in client
 *      -add single/continous mode bit (boolean)
 *      -work in fired parameter
 *      -deactivate method
 */
