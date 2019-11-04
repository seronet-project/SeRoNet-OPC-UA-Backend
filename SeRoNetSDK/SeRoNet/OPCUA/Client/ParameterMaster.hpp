/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#ifndef SERONETSDK_PARAMETERMASTER_HPP
#define SERONETSDK_PARAMETERMASTER_HPP

#include <mutex>
#include "../../Utils/Component.hpp"
#include "../../DefaultCommObjects/ParameterRequest.hpp"
#include "../../DefaultCommObjects/ParameterResponse.hpp"

#include "../../DefaultCommObjects/Description/ParameterRequestDescription.hpp"
#include "../../DefaultCommObjects/Description/ParameterResponseDescription.hpp"

#include "QueryClient.hpp"

namespace SeRoNet {
namespace OPCUA {
namespace Client {

/** Master part of parameter pattern.
   *
   *  Provides a generic port to send configuration sets to components in the system.
   *  Typically the master part is used within the sequencer. For each point in time
   *  there is a point-to-point connection between one Parameter Master and one Parameter Slave.
   *  In total one Parameter Master can send parameter-sets to several Parameter Slaves.
   *  There are three different types of parameters (which are encoded in the CommParameterRequest):
   *  - Simple parameters such as SETPOSE(x,y) CHANGEMAP("MapName"), ...
   *  - Triggers
   *  - COMMIT is a special kind of trigger that tells a component that a sequence of parameter
   *    sets is complete and the component can from now on use the consistent set of new parameters.
   *  <br>
   *
   */
class ParameterMaster {
 private:
  std::mutex mutex;

  /// management class of the component
  Smart::IComponent *component;

  SeRoNet::OPCUA::Client::QueryClient<SeRoNet::DefaultCommObjects::CommParameterRequest,
                                      SeRoNet::DefaultCommObjects::CommParameterResponse>
      query_client;

  /// internal state variable detecting disabled NamingService
  bool _use_ns;

 public:
  /** Constructor.
   *
   *  @param component management class of the component
   *
   *  (Currently exception not thrown)
   *
   */
  explicit ParameterMaster(Utils::Component *component);

  /** Destructor.
   */
  virtual ~ParameterMaster() = default;

  /** Blocking call to send a parameter set to a component indicated by the comp_name.
   *
   *  Connect the ParameterMaster to the ParameterSlave indicated by the <I>comp_name</I>
   *  and the optional <I>service_name</I>. Then, send the parameter set stored in the
   *  <I>request</I> communication object. After that, await the answer which is written
   *  into the <I>response</I> communication object.
   *
   *  @param request      contains the parameter-set to transfer to the ParameterSlave
   *  @param response     contains the response from the ParameterSlave
   *  @param comp_name    name of the ParameterSlave component
   *  @param service_name (optional) name of the service in component <I>comp_name</I> to connect to
   *
   *  @return status code:
   *    - SMART_OK                  : everything is ok and the requested connection has been
   *                                  established successfully. An old connection is first
   *                                  removed before a new connection is established.
   *    - SMART_CANCELLED           : blocking is not allowed or is not allowed anymore and
   *                                  therefore blocking call is aborted. The port of the
   *                                  slave component can now be either unchanged, disconnected
   *                                  or properly connected to the specified service provider.
   *    - SMART_UNKNOWNCOMPONENT    : the addressed slave component is either not known or does
   *                                  not provide a Parameter service. The requested connection can
   *                                  not be established.
   *    - SMART_UNKNOWNPORT         : the specified port name is not known at the slave component.
   *                                  The requested connection can not be established.
   *    - SMART_SERVICEUNAVAILABLE  : the slave component can not connect to the specified service
   *                                  of the specified server since the server and/or the service
   *                                  to be connected to from the slave component is not available.
   *    - SMART_INCOMPATIBLESERVICE : The service behind the specified port is not compatible with
   *                                  the service to be connected to. The requested connection can
   *                                  not be established.
   *    - SMART_ERROR_COMMUNICATION : communication problems either while connecting to the slave
   *                                  or at the slave component while it tried to establish the
   *                                  requested connection to the requested service provider. The
   *                                  port of the slave component can now either be unchanged,
   *                                  disconnected or already properly connected to the specified
   *                                  service provider.
   *    - SMART_ERROR               : something went completely wrong, see <I>SMART_ERROR_COMMUNICATION</I>
   */
  Smart::StatusCode sendParameterWait(
      const SeRoNet::DefaultCommObjects::CommParameterRequest &request,
      SeRoNet::DefaultCommObjects::CommParameterResponse &response,
      const std::string &comp_name, const std::string &service_name = "param");

};

} // end namespace client
} // end namespace OPCUA
} // end namespace SeRoNet
#endif //SERONETSDK_PARAMETERMASTER_HPP
