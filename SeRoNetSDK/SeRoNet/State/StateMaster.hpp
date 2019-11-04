/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#pragma once

#include "../../Definitions.hpp"
#include "CommObjs.hpp"
#include "../Exceptions/SeRoNetSDKException.hpp"
#include <smartStatusCode.h>
#include <smartIComponent.h>
#include "../OPCUA/Client/QueryClient.hpp"

namespace SeRoNet {

namespace State {

/** Master part of state pattern.
   *
   *  The configuration class provides priorized mutexes for protecting
   *  configurations. Normally, a complex robotic system requires a lot of
   *  different components which can not all run at the same time. Depending
   *  on the current task to be executed and depending on the available
   *  ressources, one has to be able to activate and deactivate components
   *  and activities within components. One the one hand deactivation should
   *  be done in such a way that the component can perform cleanup tasks.
   *  On the other hand a component must still be interruptable even if it
   *  is pending in a blocking call because the corresponding server has
   *  already been deactivated.
   *
   *  The configuration class therefore provides mechanisms for graceful
   *  activation and deactivation of sets of activities within components.
   *
   *  As is explained in the specification of the state pattern, one can
   *  activate one mainstate at a time. A mainstate can however comprise
   *  several substates which are all active at the same time. A master
   *  can only set mainstates whereas the slave can only acquire substates.
   *  This allows the simple management of sets of concurrent activities.
   *
   *  For a complete description see the printed documentation and further
   *  details in <a href="/drupal/?q=node/51#third-example">third example</a>.
   *
   */
class SERONETSDK_EXPORT StateMaster {
 private:
  typedef OPCUA::Client::QueryClient<CommStateRequest, CommStateResponse> queryClient_t;
  /// mutexConnection protects critical sections from being executed in
  ///                 parallel. These are all related to changing connections
  ///                 while client/server interactions are active. The mutex
  ///                 avoids racing conditions when for example a subscribe
  ///                 and a disconnect are called in parallel.
  std::recursive_mutex mutexConnection;

  // management class of the component
  Smart::IComponent *component;

  //<alexej date="2009-10-27">
  /// Main Query port - handles all communication of state pattern
  queryClient_t state_proxy;
  //</alexej>

  /// 0/1 to indicate whether client is not connected / is connected to server
  int statusConnected;

  /// denotes the name of the port if client can be wired from other components
  std::string portname;

  /// Default constructor
  ///
  /// throws exception if someone tries to use this constructor
  ///   SMART_ERROR : this constructor is forbidden
  StateMaster();

 public:
  // Initialization
  /** Constructor (not wired with service provider and not exposed as port).
   *  add()/remove() and connect()/disconnect() can always be used to change
   *  the status of the instance. Instance is not connected to a service provider
   *  and is not exposed as port wireable from outside the component.
   *
   *  (Currently exception not thrown)
   *
   * @param component  management class of the component
   */
  StateMaster(Utils::Component *component); // throw(Exceptions::SeRoNetSDKException) is deprecated in C++11

  /// Destructor
  virtual ~StateMaster(); // throw() is deprecated in C++11

  /** Allow or abort and reject blocking calls.
   *
   *  If blocking is set to false all blocking calls return with SMART_CANCELLED. This can be
   *  used to abort blocking calls.
   *
   *  @param b (blocking)  true/false
   *
   *  @return status code
   *   - SMART_OK                  : new mode set
   *   - SMART_ERROR               : something went wrong
   */
  Smart::StatusCode blocking(const bool b); // throw() is deprecated in C++11

  /** Blocking call to change the main state.
   *
   *  @param state
   *     - If "Deactivated" is specified as main state, every blocking
   *       query etc. is aborted with SMART_CANCELLED to reach the
   *       "neutral" state as fast as possible.
   *     - If "Neutral" is specified the neutral state will be
   *       reached without aborting pending communication
   *       calls.
   * @param server allows to define the slave for the state-change
   * @param service is an optional parameter in case the slave has a different service name than "state" (mostly for backwards compatibility)
   *
   * @return status code
   *   - SMART_OK                  : everything is ok
   *   - SMART_UNKNOWNSTATE        : requested mainstate is unknown and
   *                                 therefore no state change has been
   *                                 performed
   *   - SMART_NOTACTIVATED        : state object of server component not yet
   *                                 activated
   *   - SMART_CANCELLED           : component has been requested to reach
   *                                 neutral state, therefore pending
   *                                 setWaitState has been aborted
   *   - SMART_ERROR_COMMUNICATION : communication problems
   *   - SMART_ERROR               : something went wrong
   */
  Smart::StatusCode setWaitState(const std::string &state,
                                 const std::string &server,
                                 const std::string &service = "state");

  /** Blocking call which returns the currently active main state.
   *
   *  Is only for convenience and is not necessary since the
   *  master always knows what has been configured.
   *
   * @param MainState is set to the current main state as returned by the server (slave)
   * @param server allows to define the slave from which the currently active mainstate is returned
   * @param service is an optional parameter in case the slave has a different service name than "state" (mostly for backwards compatibility)
   *
   * @return status code
   *   - SMART_OK                  : everything is ok
   *   - SMART_NOTACTIVATED        : state object of server component not
   *                                 yet activated
   *   - SMART_ERROR_COMMUNICATION : communication problems
   *   - SMART_ERROR               : something went wrong
   *   - (hint for experts:
   *         SMART_CANCELLED can not be returned since a component can
   *         always return its current state even if it is disabled.
   *         Therefore there is no need to abort this method call when
   *         a request to reach the neutral state is pending)
   */
  Smart::StatusCode getCurrentMainState(std::string &MainState,
                                        const std::string &server,
                                        const std::string &service = "state");

  /** Blocking call which returns a list of valid main states.
   *
   *  @param MainStates contain all valid main states of the server (slave)
   * @param server allows to define the slave from which all the mainstates are returned
   * @param service is an optional parameter in case the slave has a different service name than "state" (mostly for backwards compatibility)
   *
   *  @return status code
   *   - SMART_OK                  : everything is ok
   *   - SMART_NOTACTIVATED        : state object of server component not
   *                                 yet activated
   *   - SMART_ERROR_COMMUNICATION : communication problems
   *   - SMART_ERROR               : something went wrong
   *   - (hint for experts:
   *         SMART_CANCELLED can not be returned since a component can
   *         always return its list of mainstates even if it is disabled.
   *         Therefore there is no need to abort this method call when
   *         a request to reach the neutral state is pending)
   */
  Smart::StatusCode getAllMainStates(std::list<std::string> &MainStates,
                                     const std::string &server,
                                     const std::string &service = "state");

  /** Blocking call which returns a list of substates assigned to the
   *  specified main state.
   *
   *  @param MainState is the mainstate to which we want to get the list
   *                   of substates
   *  @param SubStates    is set to the returned list of substates
   * @param server allows to define the slave from which all the substates (for the given mainstate) are returned
   * @param service is an optional parameter in case the slave has a different service name than "state" (mostly for backwards compatibility)
   *
   *  @return status code
   *   - SMART_OK                  : everything is ok
   *   - SMART_NOTACTIVATED        : state object of server component not
   *                                 yet activated
   *   - SMART_UNKNOWNSTATE        : the main state asked for is unknown
   *   - SMART_ERROR_COMMUNICATION : communication problems
   *   - SMART_ERROR               : something went wrong
   *   - (hint for experts:
   *         SMART_CANCELLED can not be returned since a component can
   *         always return its list of mainstates even if it is disabled.
   *         Therefore there is no need to abort this method call when
   *         a request to reach the neutral state is pending)
   */
  Smart::StatusCode getSubStates(const std::string &MainState,
                                 std::list<std::string> &SubStates,
                                 const std::string &server,
                                 const std::string &service = "state");
};

}
}
