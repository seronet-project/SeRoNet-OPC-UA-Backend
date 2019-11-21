/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

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
#include "../../DefaultCommObjects/ParameterRequest.hpp"
#include "../../DefaultCommObjects/ParameterResponse.hpp"
#include "../../DefaultCommObjects/Description/ParameterRequestDescription.hpp"
#include "../../DefaultCommObjects/Description/ParameterResponseDescription.hpp"

#include "QueryServerHandler.hpp"
#include "ParameterUpdateHandler.hpp"

#include "../../Utils/HsUlm/smartProcessingPattern.hpp"

namespace SeRoNet {
namespace OPCUA {
namespace Server {

class ParameterSlave {
 private:
  std::mutex mutex;

  /// management class of the component
  Smart::IComponent *component;

  class ParameterQueryHandler
      : public QueryServerHandler<SeRoNet::DefaultCommObjects::CommParameterRequest,
                                  SeRoNet::DefaultCommObjects::CommParameterResponse> {
   private:
    /// used to access the ParameterSlave from the handler
    ParameterUpdateHandler *param_handler;

   public:
    /** Constructor.
     *
     * @param param_handler
     */
    ParameterQueryHandler(
        ParameterUpdateHandler *param_handler) noexcept;

    /// Destructor
    ~ParameterQueryHandler() noexcept override = default;

    /// handle query method of query handler class
    void handleQuery(IQueryServer& server, const Smart::QueryIdPtr &id,
                     const SeRoNet::DefaultCommObjects::CommParameterRequest &request) noexcept final;
  }; // end class ParameterQueryHandler

  /// query server part
  QueryServer<SeRoNet::DefaultCommObjects::CommParameterRequest,
              SeRoNet::DefaultCommObjects::CommParameterResponse> query_server;

  ///
  std::shared_ptr<ParameterQueryHandler> query_handler;

  /// Decorator for ParameterHandler
  std::shared_ptr<SeRoNet::Utils::HsUlm::ThreadQueueQueryHandler<SeRoNet::DefaultCommObjects::CommParameterRequest,
                                                 SeRoNet::DefaultCommObjects::CommParameterResponse>>
      thread_handler;

 public:
  /** constructor.
   *
   *  (Currently exception not thrown)
   */
  ParameterSlave(Utils::Component *comp,
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
