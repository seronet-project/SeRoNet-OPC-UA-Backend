/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#pragma once

#include "../../../Definitions.hpp"
#include "AsyncAnswerMethod.hpp"
#include "InstanceStorage.hpp"
#include "../Converter/CommObjectToUaVariantArray.hpp"
#include "../Converter/UaVariantArrayToCommObject.hpp"
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

  virtual ~AsyncAnswerMethodCommObjectRequest() = default;
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
        open62541::UA_NodeId((UA_UInt16)0, (UA_UInt32) UA_NS0ID_OBJECTSFOLDER),
        methodNodeId,
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
    ss <<"Receive nongood result: " <<UA_StatusCode_name(result);
    this->setError(ss.str());
    return;
  }
  Converter::UaVariantArrayToCommObject(*outputs, CommunicationObjects::Description::SelfDescription(&m_answer, "").get());
  this->setHasAnswer();
}

template<>
SERONETSDK_EXPORT void AsyncAnswerMethodCommObjectRequest<void *>::processAnswer(
    UA_StatusCode result, open62541::UA_ArrayOfVariant *outputs);

}
}
}
