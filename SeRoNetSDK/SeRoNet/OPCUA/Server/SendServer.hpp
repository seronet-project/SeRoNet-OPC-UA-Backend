/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#pragma once

#include <Open62541Cpp/UA_ArrayOfArgument.hpp>
#include <Open62541Cpp/UA_ArrayOfVariant.hpp>
#include <thread>
#include <sstream>
#include <smartISendServerPattern_T.h>
#include "../../Utils/Component.hpp"
#include "SeRoNet/OPCUA/Converter/CommObjectToUaArgument.hpp"
#include "SeRoNet/OPCUA/Converter/CommObjectToUaVariantArray.hpp"
#include "SeRoNet/OPCUA/Converter/UaVariantArrayToCommObject.hpp"

#include "../../CommunicationObjects/Description/SelfDescription.hpp"
#include "../../Exceptions/NotImplementedException.hpp"
#include "OpcuaServer.hpp"

namespace SeRoNet {
namespace OPCUA {
namespace Server {

template<class DataType>
class SendServer : public Smart::ISendServerPattern<DataType> {
 private:
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

 protected:
  void serverInitiatedDisconnect() override;

 public:
  using ISendServerBase = Smart::ISendServerPattern<DataType>;
  using typename ISendServerBase::ISendServerHandlerPtr;

  SendServer(Utils::Component  *component, const std::string &service, ISendServerHandlerPtr handler = nullptr)
      : ISendServerBase(component, service, handler) {

    auto server = (component)->getOpcUaServer();
    DataType inputCommObject;
    open62541Cpp::UA_ArrayOfArgument
        inputArguments =
        Converter::CommObjectToUaArgumentArray(
            CommunicationObjects::Description::SelfDescription(&inputCommObject, "input").get());

    UA_MethodAttributes methodAttibute;
    UA_MethodAttributes_init(&methodAttibute);
    methodAttibute.displayName = UA_LOCALIZEDTEXT_ALLOC("en_US", service.c_str());
    methodAttibute.executable = true;
    methodAttibute.userExecutable = true;

    std::stringstream ss;
    ss <<"85." <<service.c_str(); // @todo hier sollte eine besser dynamischer weg geunden werden.

    UA_QualifiedName qualifiedName = UA_QUALIFIEDNAME_ALLOC(server.get()->getNsIndex1(), service.c_str());
    open62541Cpp::UA_NodeId nodeId(server.get()->getNsIndex1(), ss.str().c_str());
    UA_Server_addMethodNode(
        server.get()->getServer(),
        *nodeId.NodeId,
        UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASORDEREDCOMPONENT),
        qualifiedName,
        methodAttibute,
        &methodCallback,
        inputArguments.arraySize,
        inputArguments.arguments,
        0,
        nullptr,
        this,
        NULL);
    UA_QualifiedName_clear(&qualifiedName);
    UA_MethodAttributes_clear(&methodAttibute);
  }

  /** Destructor.
   *  Properly disconnects all service requestors in case of destruction
   *  such that all pending sends are handled correctly at client side
   *  even when the service provider disappears during pending sends.
   */
  virtual ~SendServer() {}
};

template<typename DataType>
inline UA_StatusCode SendServer<DataType>::methodCallback(
    UA_Server * /*server*/,
    const UA_NodeId * /*sessionId*/,
    void * /*sessionContext*/,
    const UA_NodeId * /*methodId*/,
    void *methodContext,
    const UA_NodeId */*objectId*/,
    void */*objectContext*/,
    size_t inputSize,
    const UA_Variant *input,
    size_t /*outputSize*/,
    UA_Variant */*output*/) {
  SendServer<DataType> *friendThis = static_cast<SendServer<DataType> *>(methodContext);

  DataType update;

  OPCUA::Converter::UaVariantArrayToCommObject
      conv(open62541Cpp::UA_ArrayOfVariant(input, inputSize),
           CommunicationObjects::Description::SelfDescription(&update, "").get());
  friendThis->handleSend(update);
  friendThis->notify_input(update);
  return UA_STATUSCODE_GOOD;
}

template<class DataType>
void SendServer<DataType>::serverInitiatedDisconnect() {
  throw SeRoNet::Exceptions::NotImplementedException(__FUNCTION__);
}

} //  namespace Server
} //  namespace OPCUA
} //  namespace SeRoNet
