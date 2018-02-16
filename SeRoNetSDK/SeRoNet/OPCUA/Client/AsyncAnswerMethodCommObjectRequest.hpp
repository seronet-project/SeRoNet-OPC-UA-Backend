///
/// \file AsyncAnswerMethodCommObjectRequest.hpp
/// \author Christian von Arnim
/// \date 16.02.2018
///

#pragma once

#include "AsyncAnswerMethod.hpp"
#include "InstanceStorage.hpp"
#include "../../CommunicationObjects/ICommunicationObject.hpp"
#include "Converter/CommObjectToUaVariantArray.hpp"
#include "Converter/UaVariantArrayToCommObject.hpp"

namespace SeRoNet {
namespace OPCUA {
namespace Client {
template<typename T_RETURN>
class AsyncAnswerMethodCommObjectRequest : public AsyncAnswerMethod<T_RETURN> {
 public:
  AsyncAnswerMethodCommObjectRequest(SeRoNet::OPCUA::Client::IBlocking::instanceStorage_t *instStorage,
                                           bool blockingEnabled,
                                           UA_Client *client,
                                           CommunicationObjects::ICommunicationObject *request);
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
    CommunicationObjects::ICommunicationObject *request) :
    AsyncAnswerMethod<T_RETURN>::AsyncAnswerMethod(
        instStorage,
        blockingEnabled,
        client,
        UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
        UA_NODEID_STRING_ALLOC(1, "newCoordinateService"),
        static_cast<open62541::UA_ArrayOfVariant> (
            Converter::CommObjectToUaVariantArray(request->getObjectDescription("").get())
        )
    ) {

}
template<typename T_RETURN>
void AsyncAnswerMethodCommObjectRequest<T_RETURN>::processAnswer(
    UA_StatusCode result, open62541::UA_ArrayOfVariant *outputs) {
  Converter::UaVariantArrayToCommObject(*outputs, m_answer.getObjectDescription("").get());
  this->setHasAnswer();
}

}
}
}
