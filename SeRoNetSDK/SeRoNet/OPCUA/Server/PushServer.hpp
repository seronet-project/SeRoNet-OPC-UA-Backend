/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#ifndef PUSH_SERVER_HPP
#define PUSH_SERVER_HPP

#include <iostream>
#include <unordered_map>
#include <open62541/server.h>
#include <Open62541Cpp/UA_ArrayOfVariant.hpp>
#include "../../Utils/Component.hpp"
#include <smartStatusCode.h>
#include <smartIPushServerPattern_T.h>
#include "../Converter/CommObjectToPushModel.hpp"
#include "../Converter/CommObjectToUaVariantArray.hpp"
#include "PushServerUpdater.hpp"
#include "../../CommunicationObjects/Description/SelfDescription.hpp"
#include "../../Exceptions/NotImplementedException.hpp"
#include "OpcuaServer.hpp"
#include "PushServerEnabler.hpp"

namespace SeRoNet {
namespace OPCUA {
namespace Server {

template<class T_AnswerType>
class PushServer :
    public Smart::IPushServerPattern<T_AnswerType> {
 private:
  /// management class of the component

  /// name of service
  std::string m_service;

 public:
  /** Constructor.
  *
  * @param component  management class of the component
  * @param service    name of the service
  *
  */
  PushServer(SeRoNet::Utils::Component *component, const std::string &serviceName);

  /** Destructor.
  *  Properly disconnects all service requestors in case of destruction
  *  such that all connected and subscribed clients are unsubscribed and
  *  disconnected properly.
  */
  virtual ~PushServer() = default;

  /** Provide new data which is sent to all subscribed clients
  *  taking into account their individual update cycles. Update
  *  cycles are always whole-numbered multiples of the server
  *  update cycle.
  *
  *  (Individual update interval counters are incremented each
  *   time this member function is called irrespectively of the
  *   elapsed time. One should use the time triggered handler to
  *   call the put() member function with the appropriate timing.)
  *
  *  PushHandler::handlePushRequest() directly or indirectly
  *  use this method to provide new data.
  *
  *  @param d contains the newly acquired data to be sent as
  *           update.
  *
  *  @return status code
  *    - SMART_OK                  : everything is ok
  *    - SMART_NOTACTIVATED        : server is stopped and does therefore not
  *                                  distribute any data to clients. In that
  *                                  case update interval counters are not
  *                                  touched.
  *    - SMART_ERROR_COMMUNICATION : communication problems caused by at least
  *                                  one client. The other clients are updated
  *                                  correctly.
  *    - SMART_ERROR               : something went completely wrong with at least one
  *                                  client. Some clients might still been
  *                                  updated correctly.
  */
  Smart::StatusCode put(const T_AnswerType &d) override;

 protected:
  Utils::Component * m_pComponent;
  void serverInitiatedDisconnect() override;
  bool m_FirstTimePut = {true};
};

template<class T_AnswerType>
inline PushServer<T_AnswerType>::PushServer(SeRoNet::Utils::Component *component,
                                            const std::string &serviceName) :
    Smart::IPushServerPattern<T_AnswerType>::IPushServerPattern(component, serviceName),
    m_service(serviceName), m_pComponent(component) {
  open62541Cpp::UA_NodeId
      objectsFolderNodeId(m_pComponent->getOpcUaServer()->getNsIndex0(), UA_NS0ID_OBJECTSFOLDER);

  T_AnswerType CommObject;

  Converter::CommObjectToPushModel(
      m_pComponent->getOpcUaServer(),
      CommunicationObjects::Description::SelfDescription(&CommObject, serviceName).get(),
      objectsFolderNodeId);

}

template<class T_AnswerType>
Smart::StatusCode PushServer<T_AnswerType>::put(const T_AnswerType &d) {
  T_AnswerType a(d);
  open62541Cpp::UA_NodeId
      objectsFolderNodeId(m_pComponent->getOpcUaServer()->getNsIndex0(), UA_NS0ID_OBJECTSFOLDER);

  if (m_FirstTimePut) {
    PushServerEnabler(
        m_pComponent->getOpcUaServer(),
        CommunicationObjects::Description::SelfDescription(&a, this->serviceName).get(),
        objectsFolderNodeId);
    m_FirstTimePut = false;
  }

  PushServerUpdater(
      m_pComponent->getOpcUaServer(),
      CommunicationObjects::Description::SelfDescription(&a, this->serviceName).get(),
      objectsFolderNodeId);

  return Smart::SMART_OK;
}

template<class T_AnswerType>
void PushServer<T_AnswerType>::serverInitiatedDisconnect() {
  throw SeRoNet::Exceptions::NotImplementedException(__FUNCTION__);
}

}
}
}

#endif
