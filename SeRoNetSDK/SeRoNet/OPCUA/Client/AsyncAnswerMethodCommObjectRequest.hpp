///
/// \file AsyncAnswerMethodCommObjectRequest.hpp
/// \author Christian von Arnim
/// \date 16.02.2018
///

#pragma once

#include "AsyncAnswerMethod.hpp"
#include "InstanceStorage.hpp"
#include "Converter/CommObjectToUaVariantArray.hpp"
#include "Converter/UaVariantArrayToCommObject.hpp"
#include "../../CommunicationObjects/Description/IDescription.hpp"
#include "../../CommunicationObjects/Description/SelfDescription.hpp"
#include <sstream>
#include <Open62541Cpp/UA_NodeId.hpp>

namespace SeRoNet {
namespace OPCUA {
namespace Client {
template<typename T_RETURN>
class AsyncAnswerMethodCommObjectRequest : public AsyncAnswerMethod<T_RETURN> {
 public:
  AsyncAnswerMethodCommObjectRequest(SeRoNet::OPCUA::Client::IBlocking::instanceStorage_t *instStorage,
                                     bool blockingEnabled,
                                     UA_Client *client,
                                     CommunicationObjects::Description::IDescription::shp_t requestDescription,
                                     open62541::UA_NodeId methodNodeId);
  void processAnswer(UA_StatusCode result, open62541::UA_ArrayOfVariant *outputs) override;
 protected:
  T_RETURN getAnswer() override { return m_answer; };
  T_RETURN m_answer;
};
template<typename T_RETURN>
AsyncAnswerMethodCommObjectRequest<T_RETURN>::AsyncAnswerMethodCommObjectRequest(
    SeRoNet::OPCUA::Client::IBlocking::instanceStorage_t *instStorage,
    bool blockingEnabled,
    UA_Client *client,
    CommunicationObjects::Description::IDescription::shp_t requestDescription,
    open62541::UA_NodeId methodNodeId) :
    AsyncAnswerMethod<T_RETURN>::AsyncAnswerMethod(
        instStorage,
        blockingEnabled,
        client,
        UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
        *methodNodeId.NodeId,
        static_cast<open62541::UA_ArrayOfVariant> (
            Converter::CommObjectToUaVariantArray(requestDescription.get())
        )
    ) {

}

template<typename T_RETURN>
void AsyncAnswerMethodCommObjectRequest<T_RETURN>::processAnswer(
    UA_StatusCode result, open62541::UA_ArrayOfVariant *outputs) {


  ///\todo move check to AsyncAnswerMethod?
  if(result != UA_STATUSCODE_GOOD)
  {
    std::stringstream ss;
    ss << "Receive nongood result: " << UA_StatusCode_name(result);
    this->setError(ss.str());
    return;
  }
  Converter::UaVariantArrayToCommObject(*outputs, CommunicationObjects::Description::SelfDescription(&m_answer, "").get());
  this->setHasAnswer();
}

template<>
void AsyncAnswerMethodCommObjectRequest<void *>::processAnswer(
    UA_StatusCode result, open62541::UA_ArrayOfVariant *outputs);

}
}
}
