/*
* @author Sebastian Friedl
*/

#ifndef PUSH_SERVER_HPP
#define PUSH_SERVER_HPP

#include <iostream>
#include <unordered_map>
#include <open62541.h>
#include <Open62541Cpp/UA_ArrayOfVariant.hpp>
#include "../../Utils/Component.hpp"
#include "../../../../SmartSoftComponentDeveloperAPIcpp/SmartSoft_CD_API/smartStatusCode.h"
#include "../../../../SmartSoftComponentDeveloperAPIcpp/SmartSoft_CD_API/smartIPushServerPattern_T.h"
#include "CommObjectToPushModell.hpp"
#include "../Client/Converter/CommObjectToUaVariantArray.hpp"
#include "PushServerUpdater.hpp"
#include "../../CommunicationObjects/Description/SelfDescription.hpp"
#include "../../Exceptions/NotImplementedException.hpp"
#include "OpcuaServer.hpp"

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
  void serverInitiatedDisconnect() override;
};

template<class T_AnswerType>
inline PushServer<T_AnswerType>::PushServer(SeRoNet::Utils::Component *component,
                                            const std::string &serviceName) :
    Smart::IPushServerPattern<T_AnswerType>::IPushServerPattern(component, serviceName),
    m_service(serviceName) {
  UA_Server *server = component->getServer();
  OPEN_65241_CPP_NAMESPACE::UA_NodeId objectsFolderNodeId(component->getNsIndex0(), UA_NS0ID_OBJECTSFOLDER);

  T_AnswerType *CommObject = new T_AnswerType;

  CommObjectToPushModell(
      CommunicationObjects::Description::SelfDescription(CommObject, serviceName).get(),
      OpcUaServer::instance().getServer(),
      objectsFolderNodeId,
      OpcUaServer::instance().getNsIndex1());

}

template<class T_AnswerType>
Smart::StatusCode PushServer<T_AnswerType>::put(const T_AnswerType &d) {
  T_AnswerType a(d);
  OPEN_65241_CPP_NAMESPACE::UA_NodeId objectsFolderNodeId((UA_UInt16) 0, UA_NS0ID_OBJECTSFOLDER);
  auto localComponent = dynamic_cast<SeRoNet::Utils::Component *>(this->icomponent);

  PushServerUpdater(
      CommunicationObjects::Description::SelfDescription(&a, this->serviceName).get(),
      OpcUaServer::instance().getServer(),
      objectsFolderNodeId,
      OpcUaServer::instance().getNsIndex1());

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