//
// Created by ac120371 on 09.08.2018.
//
/** Slave part of Parameter pattern.
 *
 *  The slave part of the Parameter pattern is responsible for changing the
 *  port connections within the slave components. It manages the port list
 *  of a component.
 *
 *  The user simply needs exactly one instance of the Parameter slave per
 *  component. The ParameterSlave provides no user accessible member functions.
 *
 *  Demonstrated in <a href="/drupal/?q=node/51#ninth-example">ninth example</a>
 */

#ifndef SERONETSDK_PARAMETERSLAVE_HPP
#define SERONETSDK_PARAMETERSLAVE_HPP

//  The interaction of the master / slave parts of the Parameter pattern is
//  based on the query communication pattern using the <I>CommParameter</I>
//  communication object. The service is named <I>Parameter</I>.
//
//  The port list contains the port name together with pointers to static
//  internalConnect() and internalDisconnect() methods of the client objects.
//  Those pointers are set when a client is added to the set of ports. They
//  are used to call the connect() / disconnect() methods of the client objects
//  when Parameter changes have to be performed.

#include <mutex>
#include "../../Utils/Component.hpp"
#include "../../CommunicationObjects/DefaultObjects/ParameterRequest.h"
#include "../../CommunicationObjects/DefaultObjects/ParameterResponse.h"
#include "QueryServerHandler.hpp"
#include "ParameterUpdateHandler.hpp"

#include "../../Utils/HsUlm/smartProcessingPattern.hh"

namespace SeRoNet {
namespace OPCUA {
namespace Server {

class ParameterSlave {
 private:
  std::mutex mutex;

  /// management class of the component
  SeRoNet::Utils::Component *component;

  class ParameterQueryHandler
      : public QueryServerHandler<SeRoNet::CommunicationObjects::DefaultObjects::CommParameterRequest,
                                  SeRoNet::CommunicationObjects::DefaultObjects::CommParameterResponse> {
   private:
    /// used to access the ParameterSlave from the handler
    ParameterUpdateHandler *param_handler;

   public:
    /** Constructor.
     *
     * @param slave  <I>ParameterSlave</I> needed to access it from the handler
     */
    ParameterQueryHandler(
        QueryServer<SeRoNet::CommunicationObjects::DefaultObjects::CommParameterRequest,
                    SeRoNet::CommunicationObjects::DefaultObjects::CommParameterResponse> *server,
        ParameterUpdateHandler *param_handler) noexcept;

    /// Destructor
    ~ParameterQueryHandler() noexcept override = default;

    /// handle query method of query handler class
    void handleQuery(const int &id,
                     const SeRoNet::CommunicationObjects::DefaultObjects::CommParameterRequest &request) noexcept final;
  }; // end class ParameterQueryHandler

  /// query server part
  QueryServer<SeRoNet::CommunicationObjects::DefaultObjects::CommParameterRequest,
              SeRoNet::CommunicationObjects::DefaultObjects::CommParameterResponse> query_server;

  ///
  ParameterQueryHandler query_handler;

  /// Decorator for ParameterHandler
  SeRoNet::Utils::HsUlm::ThreadQueueQueryHandler<SeRoNet::CommunicationObjects::DefaultObjects::CommParameterRequest,
                                                 SeRoNet::CommunicationObjects::DefaultObjects::CommParameterResponse>
      thread_handler;

 public:
  /** constructor.
   *
   *  (Currently exception not thrown)
   */
  ParameterSlave(SeRoNet::Utils::Component *comp,
                 ParameterUpdateHandler *hnd,
                 std::string slave_address = "param"); //FIXME wieso hier diese Default?

  /** destructor.
   *
   */
  virtual ~ParameterSlave() = default;
};

}
}
}

#endif //SERONETSDK_PARAMETERSLAVE_HPP
