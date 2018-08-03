///
/// \file StateSlave.hpp
/// \author Christian von Arnim
/// \date 30.07.2018
///

#pragma once
namespace SeRoNet {
namespace State {
  class StateSlave;
}
}

#include <string>
#include "../OPCUA/Server/QueryServer.hpp"
#include <vector>
#include <mutex>
#include "CommObjs.hpp"
#include "StateUpdateThread.hpp"
#include "StateChangeHandler.hpp"
#include <list>
#include "../../../SmartSoftComponentDeveloperAPIcpp/SmartSoft_CD_API/smartIComponent.h"
#include <condition_variable>
#include "StateSlaveHandler.hpp"

namespace SeRoNet {

namespace State {

/// \FIXME Refactoring needed! Create classes for States (Attributes for predefined and other special cases!), Substates, ThreadSafe storing, Interaction (new StateSlave class)
///         Do manimpulations of component* outside of this class (Use State Change handler)

class StateSlave {

  friend class SmartComponent;
  friend class StateSlaveHandler;

 private:
  typedef int QueryId;
  typedef OPCUA::Server::QueryServer<CommStateRequest, CommStateResponse> queryServer_t;
  typedef std::recursive_mutex StateSlaveMutex_t;

  enum StateSlaveAction {
    SSA_UNDEFINED,
    SSA_CHANGE_STATE
  };

  struct SmartStateEntry {
    StateSlaveAction action;
    std::string state;
    queryServer_t *server_proxy;
    QueryId qid;
    std::shared_ptr<std::condition_variable_any> cond = std::make_shared<std::condition_variable_any>();
  };

  //
  // indicates the next action to be performed on a substate
  //
  enum SmartStateAction {
    STATE_ACTION_ACTIVATE,
    STATE_ACTION_DEACTIVATE,
    STATE_ACTION_NONE
  };

  //
  // indicates the current mode of a substate
  //
  enum SmartStateMode {
    STATE_ACTIVATED,
    STATE_DEACTIVATED
  };

  //
  // used to manage the states of a component
  //
  struct SmartSubStateEntry {
    std::vector<std::string> mainstates;
    std::string name;
    //<alexej date="2010-09-08">
    //SmartCondClass        cond;
    std::shared_ptr<std::condition_variable_any> cond = std::make_shared<std::condition_variable_any>();
    //</alexej>
    int cnt;
    SmartStateAction action;
    SmartStateMode state;
  };

  StateUpdateThread stateUpdateThread;
  /// SmartSemaphore*                      stateQueueSemaphore; /// Use thread safe que instead!

  std::list<SmartStateEntry> stateQueue;

  SmartStateEntry desiredState;

  //<alexej date="2010-09-17">
  // current MainState
  std::string currentState;

  // initial MainState
  std::string initialState;
  //</alexej>

  mutable StateSlaveMutex_t mutex;

  ///
  std::list<SmartSubStateEntry> stateList;

  // set-up initial state-configuration
  void setUpInitialStateList();

  /// if flag is true => no more states can be defined
  bool running;

  // management class of the service
  Smart::IComponent *component;

  /// service name
  std::string service;

  /// handler for state changes
  StateChangeHandler *changeHandler;

  /// QueryHandler handles incomming state-requests from StateClient
  StateSlaveHandler *query_handler;

  /// QueryServer as main port of StateServer
  queryServer_t *query_server;

  /// @internal stop internal activity (used from SmartComponent during shutdown)
  void shutdown();

  /// private handler functions
  static void hndSetMainState(StateSlave *, queryServer_t *server, const QueryId &qid, const std::string &);
  static Smart::StatusCode hndGetCurrentState(void *, std::string &);
  static Smart::StatusCode hndGetMainStates(void *, std::vector<std::string> &);
  static Smart::StatusCode hndGetSubStates(void *, const std::string &, std::vector<std::string> &);

  /// Default constructor
  StateSlave();

 public:
  // internal use only
  void updateState(void);
  void updateStateFromThread(void);

  // Initialization

  /** Constructor.
   *  @param component management class of the component
   *  @param hnd       notify this handle object when state changes occur
   */
  //StateSlave(SmartComponent* component, StateChangeHandler & hnd) throw(SmartError);

  /** Constructor.
   *  @param component management class of the component
   *  @param hnd       optional callback interface which is called on each state-change
   *  @param serviceName the name for the internal communication-port (as will be visible on component hull)
   */
  StateSlave(Smart::IComponent *component,
             StateChangeHandler *hnd = NULL,
             const std::string &serviceName = "state");

  /// Destructor
  virtual ~StateSlave() throw();

  // User interface

  /** Blocking call to change the current component MainState.
   *
   *  @param MainState
   *     - StateSlave is only allowed to change the components lifecycle
   *       MainState (Alive, Shutdown and FatalError). Other MainStates are
   *       only allowed to be changed from the remote StateMaster.
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
   *   - SMART_ERROR               : something went wrong
   */
  Smart::StatusCode setWaitState(const std::string &MainState);

  /** Add the SubState to the MainState.
   *
   *  As long as the state object within a component is not
   *  activated, one can configure the name of the main states
   *  and substates which are afterwards visible externally.
   *
   *  If one or both names are used the first time the
   *  corresponding states are created automatically.
   *
   *  @param MainState is the MainState (parent)
   *  @param SubState is the SubState to be assigned to the MainState
   *
   *  @return status code
   *   - SMART_OK                  : everything is ok
   *   - SMART_ACTIVATED           : configuration can not be changed
   *                                 anymore once state object
   *                                 has been activated
   *   - SMART_NOTALLOWED          : tried to assign substates to the
   *                                 neutral state or something similar
   *                                 which is not allowed
   *   - SMART_ERROR_COMMUNICATION : communication problems
   *   - SMART_ERROR               : something went wrong
   */
  Smart::StatusCode defineStates(const std::string &MainState, const std::string &SubState) throw();

  /** Set the first MainState which is automatically entered after the pattern switched into Alive state.
   *
   *  Per default the MainState Neutral is entered after StateSlave entered Alive state. If this behavior is
   *  not wanted this method can be used to change the initial MainState (which must be one of the previously
   *  defined MainStates).
   *
   *  @param MainState is the initial MainState after reaching Alive state
   *
   *  @return status code
   *   - SMART_OK                  : everything OK
   *   - SMART_NOTALLOWED          : tried to assign substates to the
   *                                 neutral state or something similar
   *                                 which is not allowed
   *   - SMART_ERROR               : something went wrong
   */
  Smart::StatusCode setUpInitialState(const std::string &MainState = "Neutral");

  /** Returns the currently active MainState.
   *
   *  If the StateSlave is in progress of a state-change, the MainState to be entered is
   *  returned instead of the currently active MainState. This is more convenient for Monitoring
   *  state changes!
   *
   *  @return MainState string
   */
  std::string getCurrentMainState() const;

  /** Activation is necessary since otherwise no states can be set
   *  or acquired. No more state definitions are possible after
   *  activation.
   *
   *  @return status code
   *    - SMART_OK  : configuration has been activated and no more
   *                  state definitions possible
   */
  Smart::StatusCode activate() throw();

  /** Wait until specified substate is available and acquire it.
   *
   *  (hint: acquire(nonneutral) blocks until some main state is set.)
   *
   *  @param SubState is the name of the substate to be acquired.
   *
   *  @return status code
   *   - SMART_OK                  : everything is ok
   *   - SMART_UNKNOWNSTATE        : returns immediately if the requested
   *                                 state is unknown
   *   - SMART_NOTACTIVATED        : state object of component not yet
   *                                 activated
   *   - SMART_ERROR_COMMUNICATION : communication problems
   *   - SMART_ERROR               : something went wrong
   */
  Smart::StatusCode acquire(const std::string &SubState) throw();

  /** Acquire specified substate if available, otherwise return immediately.
   *
   * (hint: tryAcquire(nonneutral) can be used e.g. in a query
   *        handler to decide whether to process the query or to
   *        immediately return an appropriate status code indicating the
   *        components inactivity.)
   *
   *  @param SubState is the name of the substate to be acquired.
   *
   *  @return status code
   *   - SMART_OK                  : everything is ok
   *   - SMART_UNKNOWNSTATE        : returns immediately if the requested
   *                                 state is unknown
   *   - SMART_NOTACTIVATED        : state object of component not yet
   *                                 activated
   *   - SMART_ERROR_COMMUNICATION : communication problems
   *   - SMART_ERROR : something went wrong
   */
  Smart::StatusCode tryAcquire(const std::string &SubState) throw();

  /** Release specified substate.
   *
   *  @param SubState is the name of the substate to be released.
   *
   *  @return status code
   *   - SMART_OK                  : everything is ok
   *   - SMART_UNKNOWNSTATE        : returns immediately if the requested
   *                                 state is unknown
   *   - SMART_NOTACTIVATED        : state object of component not yet
   *                                 activated
   *   - SMART_ERROR_COMMUNICATION : communication problems
   *   - SMART_ERROR               : something went wrong
   */
  Smart::StatusCode release(const std::string &SubState) throw();
};

}
}

