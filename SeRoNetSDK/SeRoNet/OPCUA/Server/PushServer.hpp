/*
* @author Sebastian Friedl
*/

#ifndef PUSH_SERVER_HPP
#define PUSH_SERVER_HPP

#include <iostream>
#include <unordered_map>
#include <open62541.h>
#include "../../Utils/SmartComponent.hpp"
#include "../../../../SmartSoftComponentDeveloperAPIcpp/SmartSoft_CD_API/smartIStatusCode.h"
#include "../../../../SmartSoftComponentDeveloperAPIcpp/SmartSoft_CD_API/smartIPushServerPattern_T.h"

namespace SeRoNet {
namespace OPCUA {
namespace Server {

template<class T_AnswerType>
class PushServer :
    public Smart::IPushServerPattern<T_AnswerType> {
 private:
  /// management class of the component
  SeRoNet::Utils::SmartComponent *m_component;

  /// name of service
  std::string m_service;

 public:
  /** Constructor.
  *
  * @param component  management class of the component
  * @param service    name of the service
  *
  */
  PushServer(SeRoNet::Utils::SmartComponent *component, const std::string &serviceName);

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
   Smart::StatusCode put(const T_AnswerType& d) override;

 protected:
  void serverInitiatedDisconnect() override;
};

template<class T_AnswerType>
inline PushServer<T_AnswerType>::PushServer(SeRoNet::Utils::SmartComponent *component,
                                            const std::string &serviceName) :
    Smart::IPushServerPattern<T_AnswerType>::IPushServerPattern(component, serviceName),
    m_component(component),
    m_service(serviceName) {
  UA_Server *server = component->getServer();
  T_AnswerType::createPushModell(server);

}

template<class T_AnswerType>
Smart::StatusCode PushServer<T_AnswerType>::put(const T_AnswerType& d) {
    T_AnswerType a(d);
   a.putObject(this->m_component->getServer());
  return Smart::SMART_OK;
}

template<class T_AnswerType>
void PushServer<T_AnswerType>::serverInitiatedDisconnect() {
  //TODO what is here todo?
  return;
}

}
}
}

#endif