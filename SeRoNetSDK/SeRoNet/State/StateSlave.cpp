///
/// \file StateSlave.cpp
/// \author Christian von Arnim
/// \date 30.07.2018
///

/// From ACE Implementation

#include "StateSlave.hpp"
#include "CommObjsSelfDescription.hpp"
#include <iostream>

namespace SeRoNet {
namespace State {

void StateSlave::hndSetMainState(StateSlave *ptr,
                                 queryServer_t *server,
                                 const QueryId &qid,
                                 const std::string &mainstate) {
  StateSlave *lthis = (StateSlave *) ptr;
  SmartStateEntry entry;
  CommStateResponse reply;

  entry.state = mainstate;
  entry.action = SSA_CHANGE_STATE;
  entry.server_proxy = server;
  entry.qid = qid;

  std::unique_lock<decltype(lthis->mutex)> ul(lthis->mutex);

  if (lthis->running == false) {
    //
    // state management not yet activated, send immediately status
    //
    reply.setStatus(static_cast<int>(Smart::SMART_NOTACTIVATED));

    entry.server_proxy->answer(qid, reply);
    entry.server_proxy = 0;
  } else if (
      (mainstate == "Init") ||
          (mainstate == "init") ||
          (mainstate == "Alive") ||
          (mainstate == "alive") ||
          (mainstate == "FatalError") ||
          (mainstate == "fatalerror") ||
          (mainstate == "fatalError")) {

    //
    // state is not allowed to be commanded from the outside
    //
    reply.setStatus(static_cast<int>(Smart::SMART_NOTALLOWED));

    entry.server_proxy->answer(qid, reply);
    entry.server_proxy = 0;

  } else if (mainstate == "Shutdown") {
    //
    // enqueue the state change request
    //
    // TODO: check if enqueue_head is allowed
    lthis->stateQueue.push_front(entry);
  } else {

    if (lthis->currentState == "Init") {
      //
      // state management not initialized yet, send immediately status
      //
      reply.setStatus(static_cast<int>(Smart::SMART_NOTACTIVATED));
      entry.server_proxy->answer(qid, reply);
      entry.server_proxy = 0;

    } else if ((lthis->currentState == "Shutdown") || (lthis->currentState == "FatalError")) {
      //
      // state management not initialized yet, send immediately status
      //
      reply.setStatus(static_cast<int>(Smart::SMART_CANCELLED));
      entry.server_proxy->answer(qid, reply);
      entry.server_proxy = 0;

    } else {
      //
      // enqueue the state change request
      //
      lthis->stateQueue.push_back(entry);
    }
  }

}

Smart::StatusCode StateSlave::hndGetCurrentState(void *ptr, std::string &mainstate) {
  StateSlave *lthis = (StateSlave *) ptr;
  Smart::StatusCode result;

  std::unique_lock<decltype(lthis->mutex)> ul(lthis->mutex);

  if (lthis->running == false) {
    //
    // state management not yet activated
    //
    result = Smart::SMART_NOTACTIVATED;
  } else {
    mainstate = lthis->currentState;
    result = Smart::SMART_OK;
  }

  return result;
}

Smart::StatusCode StateSlave::hndGetMainStates(void *ptr, std::vector<std::string> &mainstates) {
  StateSlave *lthis = (StateSlave *) ptr;
  std::list<SmartSubStateEntry>::iterator iterator;
  std::list<std::string> stateResults;
  std::vector<std::string>::iterator mIterator;

  Smart::StatusCode result;

  std::unique_lock<decltype(lthis->mutex)> ul(lthis->mutex);

  if (lthis->running == false) {
    //
    // state management not yet activated
    //

    result = Smart::SMART_NOTACTIVATED;
  } else {
    //
    // collect all the mainstates distributed over the substates, the standard "neutral"
    // state is pre-added
    //
    std::string neutral("Neutral");

    mainstates.clear();
    mainstates.push_back(neutral.c_str());

    stateResults.push_back(neutral);

    //<alexej date="2010-09-20"
    // <descrition>
    //   All main-states are reachable through the substate "nonneutral".
    //   Thus, the loop for all substates is not necessary!
    // </description>
    //</alexej>
    for (iterator = lthis->stateList.begin(); iterator != lthis->stateList.end(); ++iterator) {
      // if nonneutral is found -> break up loop
      if (iterator->name == "nonneutral") break;
    }

    if (iterator != lthis->stateList.end()) {
      // nonneutral entry is found!

      // get all MainStates which are registered with the substate "nonneutral"
      for (mIterator = iterator->mainstates.begin(); mIterator != iterator->mainstates.end(); ++mIterator) {
        mainstates.push_back(*mIterator);
      }

      //<alexej date="2010-09-20">
      // <description>
      //  The MainStates "Init", "Shutdown", "Alive" and "FatalError" are NOT listed in this request, because:
      //  1) They are always the same and can be assumed to be there (which saves unnecessary communication)
      //  2) Besides of "Shutdown" they are not allowed to be set from the outside (shutdown is obvious like "deactivated")
      // </description>
      //</alexej>

      result = Smart::SMART_OK;
    } else {
      // nonneutral is not found (should not happen)
      result = Smart::SMART_ERROR;
    }
  }

  return result;
}

Smart::StatusCode StateSlave::hndGetSubStates(void *ptr,
                                              const std::string &mainstate,
                                              std::vector<std::string> &substates) {
  StateSlave *lthis = (StateSlave *) ptr;
  std::list<SmartSubStateEntry>::iterator iterator;
  std::vector<std::string>::iterator mIterator;
  unsigned int count = 0;
  Smart::StatusCode result;

  std::unique_lock<decltype(lthis->mutex)> ul(lthis->mutex);

  substates.clear();

  if (lthis->running == false) {
    //
    // state management not yet activated
    //

    result = Smart::SMART_NOTACTIVATED;

    //<alexej date="2010-09-20">
    // <description>Neutral is a regular MainState now and is handled together with other MainStates</description>
    /*
    } else if (mainstate == "neutral") {
      //
      // "neutral" has no substates
      //
      result = Smart::SMART_OK;
    */
    //</alexej>
  } else {
    // All SubStates for a specific MainState are now collected in the substates list.
    // There are all MainStates allowed, including "Init", "Alive", "Shutdown", "FatalError" and "Neutral".
    // Each of these MainStates has exactly one SubState (beginning with a lower case character)
    // User defined MainStates contain at least the SubState "nonneutral" in addition to user-defined SubStates.
    count = 0;
    for (iterator = lthis->stateList.begin(); iterator != lthis->stateList.end(); ++iterator) {
      for (mIterator = iterator->mainstates.begin(); mIterator != iterator->mainstates.end(); ++mIterator) {
        if (*mIterator == mainstate) {
          // found mainstate in this substate, therefore add substate to result list
          count++;
          substates.push_back(iterator->name.c_str());

          // now end for loop since we have already added this substate ...
          break;
        }
      }
    }

    if (count == 0) {
      // couldn't find the mainstate
      result = Smart::SMART_UNKNOWNSTATE;
    } else {
      result = Smart::SMART_OK;
    }
  }

  return result;
}

//
// standard constructor
//
StateSlave::StateSlave(Smart::IComponent *comp, StateChangeHandler *hnd, const std::string &serviceName)
    : running(false),
      component(comp),
      changeHandler(hnd),
      stateUpdateThread(comp)
      {
  Smart::StatusCode result = Smart::SMART_OK;

  std::unique_lock<decltype(mutex)> ul(mutex);

  if (result != Smart::SMART_OK)
    throw (Exceptions::SeRoNetSDKException(
        "CommPattern (stateServer): ERROR: Something went wrong during Acceptor initialization!\n"));

  //<alexej date="2011-02-24">
  setUpInitialStateList();

  //component->setStateSlave(this); /// \todo ersatz in IComponent?
  query_server = new queryServer_t(component, serviceName);
  query_handler = new StateSlaveHandler(this);
  //</alexej>

  stateUpdateThread.init(this);
  stateUpdateThread.start();

}

//
// default destructor
//
StateSlave::~StateSlave(void) throw() {
  shutdown();
  //delete acceptor;
  delete query_handler;
  delete query_server;

  ///component->setStateSlave(NULL); /// \todo ersatz in IComponent?

}

//
// internal use only
//
void StateSlave::shutdown() {
  std::unique_lock<decltype(mutex)> ul(mutex);
  running = false;
  stateUpdateThread.halt();
  stateUpdateThread.stop(true);
}

//
// internal use only
//
void StateSlave::setUpInitialStateList() {
  std::unique_lock<decltype(mutex)> ul(mutex);

  // 1) set-up a base set of sub-states

  // 1.1) define Init Main+SubState!
  SmartSubStateEntry substate_init;
  substate_init.name = "init";
  substate_init.mainstates.push_back("Init");
  substate_init.cnt = 0;
  substate_init.action = STATE_ACTION_NONE;
  // this SubState is activated at beginning by default
  substate_init.state = STATE_ACTIVATED;

  // Alive is a pseudo-state that has no main or substates
//   // 1.2) define alive SubState
//   SmartSubStateEntry substate_alive;
//   substate_alive.name = "alive";
//   // there is no MainState for alive. Instead it is handled as a command, like "deactivated".
//   substate_alive.cond = new SmartConditionRecursiveMutex(mutex);
//   substate_alive.cnt = 0;
//   substate_alive.action = STATE_ACTION_NONE;
//   substate_alive.state = STATE_DEACTIVATED;

  // 1.3) define Neutral Main+SubState (in contrast to former implementation, Neutral now has a corresponding SubState!)
  SmartSubStateEntry substate_neutral;
  substate_neutral.name = "neutral";
  substate_neutral.mainstates.push_back("Neutral");
  substate_neutral.cnt = 0;
  substate_neutral.action = STATE_ACTION_NONE;
  substate_neutral.state = STATE_DEACTIVATED;

  // 1.4) define FatalError Main+SubState
  SmartSubStateEntry substate_fatal;
  substate_fatal.name = "fatalError";
  substate_fatal.mainstates.push_back("FatalError");
  substate_fatal.cnt = 0;
  substate_fatal.action = STATE_ACTION_NONE;
  substate_fatal.state = STATE_DEACTIVATED;

  // 1.5) define Shutdown Main+SubState
  SmartSubStateEntry substate_shutdown;
  substate_shutdown.name = "shutdown";
  substate_shutdown.mainstates.push_back("Shutdown");
  substate_shutdown.cnt = 0;
  substate_shutdown.action = STATE_ACTION_NONE;
  substate_shutdown.state = STATE_DEACTIVATED;

  // 1.6) define nonneutral SubState
  SmartSubStateEntry nonneutral;
  nonneutral.name = "nonneutral";
  // no mainstates are assigned at the beginning
  nonneutral.cnt = 0;
  nonneutral.action = STATE_ACTION_NONE;
  nonneutral.state = STATE_DEACTIVATED;

  // 2) push all six substates on the state-list
  stateList.push_back(substate_init);
  //stateList.push_back(substate_alive);
  stateList.push_back(substate_neutral);
  stateList.push_back(substate_fatal);
  stateList.push_back(substate_shutdown);
  stateList.push_back(nonneutral);

  // 3) make Init the initial state at pattern-creation
  currentState = "Init";

  // set desired state also to Init
  desiredState.action = SSA_UNDEFINED;
  desiredState.state = "Init";
  // the proxy and query id are not used at the beginning
  desiredState.server_proxy = 0;
  desiredState.qid = 0;

  // 4) set initialMainState to "Neutral" per default
  initialState = "Neutral";

}

//
// internal use only
//
void StateSlave::updateState(void) {
  std::list<SmartSubStateEntry>::iterator sIterator;
  std::vector<std::string>::iterator mIterator;

  //<alexej date="2009-10-13">
  int currentFlag;
  int desiredFlag;
  int lockedFlag;

  CommStateResponse reply;

  int callQuitHandler;
  int callEnterHandler;
  std::list<SmartSubStateEntry>::iterator hIterator;

  int validFlag;

  std::unique_lock<decltype(mutex)> ul(mutex);

  if (desiredState.action == SSA_CHANGE_STATE) {
    //
    // there is a pending state change request
    //

    //
    // now check whether all substates to be deactivated
    // are not locked anymore. If all those substates are
    // not acquired anymore, perform the state change. Since
    // this is all done protected by the mutex, no new locks
    // on substates can be set meanwhile. If there are still
    // any substates acquired, postpone the state change.
    //
    if (desiredState.state == currentState) {
      //
      // do nothing since this state is already set
      //
      desiredState.action = SSA_UNDEFINED;

      if (desiredState.server_proxy != 0) {
        //
        // if no action is pending, one should not have a client ptr here ....
        //

        //<alexej date="2009-10-13">
        //desiredState.client->answer(Smart::SMART_OK);
        reply.setStatus(static_cast<int>(Smart::SMART_OK));
        desiredState.server_proxy->answer(desiredState.qid, reply);

        // do nothing, because release would destroy the whole object
        //CORBA::release(desiredState.client);
        //</alexej>

        desiredState.server_proxy = 0;
      }

    } else {
      //
      // check whether requested main state is valid at all
      //
      validFlag = 0;
      for (sIterator = stateList.begin(); sIterator != stateList.end(); ++sIterator) {
        for (mIterator = sIterator->mainstates.begin(); mIterator != sIterator->mainstates.end(); ++mIterator) {
          if (desiredState.state == *mIterator) {
            validFlag = 1;
          }
        }
      }

      //<alexej date="2010-09-22">
      // if ((validFlag == 1) || (desiredState.state == "neutral") || (desiredState.state == "deactivated")) {
      if ((validFlag == 1) || (desiredState.state == "deactivated") || (desiredState.state == "Deactivated")
          || (desiredState.state == "Alive")) {
        //</alexej>
        //
        // required main state is valid and current state is different from desired state
        //
        for (sIterator = stateList.begin(); sIterator != stateList.end(); ++sIterator) {
          // check which substates belong to the current and to the desired main state

          sIterator->action = STATE_ACTION_NONE;

          desiredFlag = 0;
          currentFlag = 0;
          for (mIterator = sIterator->mainstates.begin(); mIterator != sIterator->mainstates.end(); ++mIterator) {
            if (desiredState.state == *mIterator) {
              desiredFlag = 1;
            }
            if (currentState == *mIterator) {
              currentFlag = 1;
            }
          }
          if ((currentFlag == 1) && (desiredFlag == 0)) {
            // add this substate to the deactivation list
            sIterator->action = STATE_ACTION_DEACTIVATE;
          }
          if ((currentFlag == 0) && (desiredFlag == 1)) {
            // add this substate to the activation list
            sIterator->action = STATE_ACTION_ACTIVATE;
          }
        }

        //
        // now check whether all substates to be deactivated are not
        // locked anymore
        //
        lockedFlag = 0;
        for (sIterator = stateList.begin(); sIterator != stateList.end(); ++sIterator) {
          if ((sIterator->action == STATE_ACTION_DEACTIVATE) && (sIterator->cnt != 0)) {
            lockedFlag = 1;
          }
        }

        if (lockedFlag == 0) {
          // all substates to be released are not locked anymore,
          // therefore perform state change
          if ((desiredState.state == std::string("Deactivated"))
              || (desiredState.state == std::string("deactivated"))) {
            //
            // check whether to reach the neutral state has been
            // enforced. If so, again allow acquiring states which
            // does not cause any problems since states can only be
            // acquired again after we leave this completely mutex
            // protected block.
            //

            component->blocking(true);
            desiredState.state = std::string("Neutral");
          } else if ((desiredState.state == "Shutdown") || (desiredState.state == "FatalError")) {
            component->blocking(true);
          }

          // now call the quit / enter handler. The quitHandler for the nonneutral
          // substate is called AFTER every other quitHandler and the enterHandler
          // for the nonneutral substate BEFORE any other enterHandler

          std::cout << "Performing state change:" << currentState << "->" << desiredState.state << std::endl;

          callQuitHandler = 0;
          callEnterHandler = 0;
          for (sIterator = stateList.begin(); sIterator != stateList.end(); ++sIterator) {
            if (sIterator->action == STATE_ACTION_DEACTIVATE) {
              if (sIterator->name == "nonneutral") {
                // save iterator for call AFTER every deactivation
                hIterator = sIterator;
                callQuitHandler = 1;
              } else {
                if (changeHandler != NULL) changeHandler->handleQuitState(sIterator->name);
                sIterator->state = STATE_DEACTIVATED;
              }
            }
            if ((sIterator->action == STATE_ACTION_ACTIVATE) && (sIterator->name == "nonneutral")) {
              // save iterator for activation call BEFORE every other activation.
              // can use the same hIterator since the nonneutral substates occurs
              // only once in the list of substates
              hIterator = sIterator;
              callEnterHandler = 1;
            }
          }

          if (callQuitHandler == 1) {
            if (changeHandler != NULL) changeHandler->handleQuitState(hIterator->name);
            hIterator->state = STATE_DEACTIVATED;
          }
          if (callEnterHandler == 1) {
            if (changeHandler != NULL) changeHandler->handleEnterState(hIterator->name);
            hIterator->state = STATE_ACTIVATED;
          }

          for (sIterator = stateList.begin(); sIterator != stateList.end(); ++sIterator) {
            if (sIterator->action == STATE_ACTION_ACTIVATE) {
              if (sIterator->name == "nonneutral") {
                // has already been called BEFORE every other activation
              } else {
                if (changeHandler != NULL) changeHandler->handleEnterState(sIterator->name);
                sIterator->state = STATE_ACTIVATED;
              }
            }
          }

          for (sIterator = stateList.begin(); sIterator != stateList.end(); ++sIterator) {
            if (sIterator->state == STATE_ACTIVATED || desiredState.state == "Shutdown") {
              //
              // signal every blocking acquire that this substate is available now !
              //

              // see comment in acquire()-method why we signal every activated substate
              //<alexej date="2010-09-08">
              //sIterator->cond.signalAll();
              sIterator->cond->notify_all();
              //</alexej>
            }
          }

          //
          // performed state change
          //
          currentState = desiredState.state;

          desiredState.action = SSA_UNDEFINED;

          desiredState.cond->notify_all();

          if (desiredState.server_proxy != 0) {
            //
            // there is a state change pending, therefore send answer
            //
            // (Since this method is left without any action if there are still
            //  locks on required substates, we have to remember for the next call
            //  to notify the master as soon as we have finished the state change.
            //  If we arrive here, we successfully finished the state change and
            //  as long as we have a client ptr here, we know that there is a
            //  master waiting for that notification).

            //<alexej date="2009-10-27">
            reply.setStatus(static_cast<int>(Smart::SMART_OK));
            desiredState.server_proxy->answer(desiredState.qid, reply);

            // do nothing, because releasae would destroy the whole object
            //CORBA::release(desiredState.client);
            //</alexej>

            desiredState.server_proxy = 0;
          }
        }
      } else {
        //
        // requested a state change to an invalid mainstate
        //
        desiredState.action = SSA_UNDEFINED;

        if (desiredState.server_proxy != 0) {
          //
          // there is a state change pending, therefore send answer
          //
          //<alexej date="2009-10-27">
          reply.setStatus(static_cast<int>(Smart::SMART_UNKNOWNSTATE));
          desiredState.server_proxy->answer(desiredState.qid, reply);

          // do nothing, because releasae would destroy the whole object
          //CORBA::release(desiredState.client);
          //</alexej>

          desiredState.server_proxy = 0;
        }
      }
    }

  } else if (desiredState.action == SSA_UNDEFINED) {
    //
    // no pending action, nothing has to be done ...
    //
    desiredState.action = SSA_UNDEFINED;

    if (desiredState.server_proxy != 0) {
      //
      // if no action is pending, one should not have a client ptr here ....
      //

      std::cerr << "### strange: no action pending, but have client ptr" << std::endl;

      //<alexej date="2009-10-13">
      reply.setStatus(static_cast<int>(Smart::SMART_ERROR));
      desiredState.server_proxy->answer(desiredState.qid, reply);

      // do nothing, because releasae would destroy the whole object
      //CORBA::release(desiredState.client);
      //</alexej>

      desiredState.server_proxy = 0;
    }
  } else {
    //
    // unknown pending action => ignore and return error to master in case of available ptr
    //
    desiredState.action = SSA_UNDEFINED;

    if (desiredState.server_proxy != 0) {
      //
      // there is a client ptr available, therefore client wants to get informed
      //

      std::cerr << "### strange: unknown action pending" << std::endl;

      //<alexej date="2009-10-13">
      reply.setStatus(static_cast<int>(Smart::SMART_ERROR));
      desiredState.server_proxy->answer(desiredState.qid, reply);

      // do nothing, because releasae would destroy the whole object
      //CORBA::release(desiredState.client);
      //</alexej>

      desiredState.server_proxy = 0;
    }
  }

  //</alexej>
}

//
// internal use only
//
void StateSlave::updateStateFromThread(void) {
  std::string desired;

  std::unique_lock<decltype(mutex)> ul(mutex);

  //<alexej date="2009-10-13">
  if (desiredState.server_proxy != 0) {
    std::cerr << "WARNING: state change in progress (now killed)" << std::endl;
    // #### hier muß man dem alten master eine entsprechenden status als Antwort schicken !!!!
  }
  //</alexej>

  if (stateQueue.size() == 0) {
    return;
  }
  desiredState = stateQueue.front();
  stateQueue.pop_front();
  desired = desiredState.state;

  //
  // check whether to cancel waiting calls
  //
  if ((desired == "Deactivated") ||
      (desired == "Shutdown") ||
      (desired == "FatalError") ||
      (desired == "deactivated")) {
    component->blocking(false);
  }

  //
  // now perform the state update
  //
  updateState();

  if (desired == "Shutdown") {
    //shutdown();
    stateUpdateThread.halt();
  }
}

//
//
//
Smart::StatusCode StateSlave::setWaitState(const std::string &MainState) {
  // Allowed transition table:

  // Init->Alive
  // Init->Shutdown
  // Init->FatalError
  // Alive->Shutdown
  // Alive->FatalError
  // FatalError->Shutdown
  // Shutdown (no successive states)

  Smart::StatusCode result = Smart::SMART_ERROR;
  SmartStateEntry entry;

  std::unique_lock<decltype(mutex)> ul(mutex);

  if (running == false) {
    // StatePatter is not activated yet
    result = Smart::SMART_NOTACTIVATED;
  } else if ((MainState == "Alive") ||
      (MainState == "FatalError") ||
      (MainState == "Shutdown")) {
    // new MainState is allowed in principle to be set from the inside of the Pattern.
    // check if the transition is allowed according to the current MainState
    if (currentState == "Init") {
      // Allowed successor MainStates from Init are: Alive, Shutdown and FatalError

      entry.state = MainState;
      entry.action = SSA_CHANGE_STATE;
      entry.qid = 0;
      entry.server_proxy = NULL;

      // TODO: check if enqueue_head is allowed
      stateQueue.push_front(entry);

      // wait on state-change to complete
      entry.cond->wait(ul);

      if (MainState == "Shutdown") {
        // because entry.cond->wait() released, the pattern is now in Shutdown MainState
        // shutdown procedure is commanded from within the updateState method
        result = Smart::SMART_OK;
      } else if (MainState == "Alive") {
        // set-up the initialy configured MainState
        entry.state = initialState;
        entry.action = SSA_CHANGE_STATE;
        entry.qid = 0;
        entry.server_proxy = NULL;

        // put the initial state as a regular user-defined state on the queue
        stateQueue.push_back(entry);

        // wait till state change is performed
        entry.cond->wait(ul);
      }

      result = Smart::SMART_OK;

    } else if (currentState == "Shutdown") {
      if (MainState == "Shutdown") {
        result = Smart::SMART_OK;
      } else {
        // there is no transition possible out of the Shutdown state
        result = Smart::SMART_NOTALLOWED;
      }
    } else if (currentState == "FatalError") {
      // The only allowed transition from FatalError is Shutdown
      if (MainState == "Shutdown") {
        entry.state = MainState;
        entry.action = SSA_CHANGE_STATE;
        entry.qid = 0;
        entry.server_proxy = NULL;

        // TODO: check if enqueue_head is allowed
        stateQueue.push_front(entry);

        // wait on state-change to complete
        entry.cond->wait(ul);

        // because entry.cond->wait() released, the pattern is now in Shutdown MainState
        // shutdown procedure is commanded from within the updateState method
        result = Smart::SMART_OK;

      } else if (MainState == "FatalError") {
        // transition to the same state (allowed)
        result = Smart::SMART_OK;
      } else {
        // desired transition is not allowed
        result = Smart::SMART_NOTALLOWED;
      }
    } else {
      // the currentState is neither Init, Shutdown nor FatalError
      // the only other possibility is the Alive state which is a pseudo-state
      // and is not saved in the currentState variable.

      if (MainState == "Alive") {
        // we are already in Alive -> return OK
        result = Smart::SMART_OK;
      } else {
        // The new MainState can be only Shutdown or FatalError
        // on both cases simply change into the new MainState

        entry.state = MainState;
        entry.action = SSA_CHANGE_STATE;
        entry.qid = 0;
        entry.server_proxy = NULL;

        // TODO: check if enqueue_head is allowed
        stateQueue.push_front(entry);

        // wait on state-change to complete
        entry.cond->wait(ul);

        // because entry.cond->wait() released, the pattern is now in Shutdown or FatalError MainState
        // shutdown procedure is commanded from within the updateState method
        result = Smart::SMART_OK;
      }
    }

  } else {
    // desired MainState is not one of the generic lifecycle states and is thus not allowed to be changed by StateSlave
    result = Smart::SMART_NOTALLOWED;
  }

  return result;
}


//
//
//

Smart::StatusCode StateSlave::defineStates(const std::string &mainstate, const std::string &substate) throw() {
  std::list<SmartSubStateEntry>::iterator iterator;
  std::list<SmartSubStateEntry>::iterator substatePtr;
  SmartSubStateEntry newEntry;
  int countS;
  std::vector<std::string>::iterator mIterator;
  int countM;

  Smart::StatusCode result = Smart::SMART_ERROR;

  std::unique_lock<decltype(mutex)> ul(mutex);
  if (running == true) {
    result = Smart::SMART_ACTIVATED;
  } else {
    if (
        (mainstate == "Init") ||
            (mainstate == "init") ||
            (mainstate == "Alive") ||
            (mainstate == "alive") ||
            (mainstate == "Neutral") ||
            (mainstate == "neutral") ||
            (mainstate == "Shutdown") ||
            (mainstate == "shutdown") ||
            (mainstate == "FatalError") ||
            (mainstate == "fatalerror") ||
            (mainstate == "fatalError") ||
            (mainstate == "deactivated") ||
            (mainstate == "Deactivated") ||

            (substate == "Init") ||
            (substate == "init") ||
            (substate == "Alive") ||
            (substate == "alive") ||
            (substate == "neutral") ||
            (substate == "Neutral") ||
            (substate == "Shutdown") ||
            (substate == "shutdown") ||
            (substate == "FatalError") ||
            (substate == "fatalerror") ||
            (substate == "fatalError") ||
            (substate == "nonneutral") ||
            (substate == "nonNeutral") ||
            (substate == "NonNeutral")
        ) {
      //
      // not allowed to change reserved states
      //

      result = Smart::SMART_NOTALLOWED;
    } else {
      //
      // regular state ...
      //

      //
      // first check whether this mainstate is already assigned to the predefined
      // "nonneutral" substate. Every mainstate is automatically assigned to the
      // "nonneutral" substate.
      //
      countS = 0;
      /// \TODO Use std::map for easier search!
      for (iterator = stateList.begin(); iterator != stateList.end(); ++iterator) {
        if (iterator->name == "nonneutral") {
          substatePtr = iterator;
          countS++;
        }
      }
      if (countS == 0) {
        //
        // "nonneutral" substate does not exist => error
        //
        result = Smart::SMART_ERROR;
      } else if (countS == 1) {
        //
        // found substate "nonneutral", substatePtr points to it, now check whether
        // mainstate is already contained
        //
        countM = 0;
        /// \TODO Replace by std::map/ std::set for garantee unique entries
        for (mIterator = substatePtr->mainstates.begin(); mIterator != substatePtr->mainstates.end(); ++mIterator) {
          if (*mIterator == mainstate) countM++;
        }

        if (countM == 0) {
          //
          // mainstate not yet bound with this substate
          //
          substatePtr->mainstates.push_back(mainstate);

          result = Smart::SMART_OK;
        } else if (countM == 1) {
          //
          // mainstate already assigned, do nothing
          //
          result = Smart::SMART_OK;
        } else {
          //
          // mainstate exists multiple times => error
          //
          result = Smart::SMART_ERROR;
        }
      } else {
        //
        // substate exists multiple times => error
        //
        result = Smart::SMART_ERROR;
      }


      //
      // if no error occured so far continue to check the substate
      //
      if (result == Smart::SMART_OK) {

        //
        // now check whether substate is already in list
        //
        countS = 0;
        for (iterator = stateList.begin(); iterator != stateList.end(); ++iterator) {
          if (iterator->name == substate) {
            substatePtr = iterator;
            countS++;
          }
        }
        if (countS == 0) {
          //
          // substate does not yet exist, generate new entry
          //
          newEntry.name = substate;
          //<alexej date="2010-09-08">
          //</alexej>
          newEntry.cnt = 0;
          newEntry.action = STATE_ACTION_NONE;
          newEntry.state = STATE_DEACTIVATED;
          newEntry.mainstates.push_back(mainstate);

          stateList.push_back(newEntry);

          result = Smart::SMART_OK;
        } else if (countS == 1) {
          //
          // substate already exists, substatePtr points to it, now check whether mainstate
          // is already contained
          //
          countM = 0;
          for (mIterator = substatePtr->mainstates.begin(); mIterator != substatePtr->mainstates.end(); ++mIterator) {
            if (*mIterator == mainstate) countM++;
          }

          if (countM == 0) {
            //
            // mainstate not yet bound with this substate
            //
            substatePtr->mainstates.push_back(mainstate);

            result = Smart::SMART_OK;
          } else if (countM == 1) {
            //
            // mainstate already assigned, do nothing
            //
            result = Smart::SMART_OK;
          } else {
            //
            // mainstate exists multiple times => error
            //
            result = Smart::SMART_ERROR;
          }
        } else {
          //
          // substate exists multiple times => error
          //
          result = Smart::SMART_ERROR;
        }
      } // end if(countS == ...)

    } // end if((mainstate == "neutral") || (substate == "nonneutral"))
  } // end if(running)

  return result;
}

//
//
//
Smart::StatusCode StateSlave::setUpInitialState(const std::string &MainState) {
  std::list<SmartSubStateEntry>::iterator iterator;
  std::list<SmartSubStateEntry>::iterator substatePtr;
  int countS;
  std::vector<std::string>::iterator mIterator;

  Smart::StatusCode result = Smart::SMART_ERROR;

  std::unique_lock<decltype(mutex)> ul(mutex);

  if (MainState == "Neutral") {
    // neutral state is OK to be set as the initial state
    initialState = MainState;
    result = Smart::SMART_OK;
  } else {
    // search for the "nonneutral" substate in stateList
    countS = 0;
    for (iterator = stateList.begin(); iterator != stateList.end(); ++iterator) {
      if (iterator->name == "nonneutral") {
        substatePtr = iterator;
        countS++;
      }
    }
    if (countS == 0) {
      //
      // "nonneutral" substate does not exist => error
      //
      result = Smart::SMART_ERROR;
    } else if (countS == 1) {
      //
      // found substate "nonneutral", substatePtr points to it,
      // now search for the mainstate and set it if found
      //
      for (mIterator = substatePtr->mainstates.begin(); mIterator != substatePtr->mainstates.end(); ++mIterator) {
        if (*mIterator == MainState) {
          // MainState found in list -> MainState is legal -> save it as the initialState
          initialState = MainState;
          result = Smart::SMART_OK;
        }
      }

    } else {
      //
      // substate exists multiple times => error
      //
      result = Smart::SMART_ERROR;
    }
  }

  return result;
}

std::string StateSlave::getCurrentMainState() const {
  std::string retval = "";

  std::unique_lock<decltype(mutex)> ul(mutex);
  if (desiredState.action == SSA_CHANGE_STATE) {
    retval = desiredState.state;
  } else {
    retval = currentState;
  }

  return retval;
}

Smart::StatusCode StateSlave::activate() throw() {
  std::unique_lock<decltype(mutex)> ul(mutex);
  running = true;

  return Smart::SMART_OK;
}

Smart::StatusCode StateSlave::acquire(const std::string &substate) throw() {
  std::list<SmartSubStateEntry>::iterator iterator;
  std::list<SmartSubStateEntry>::iterator sIterator = stateList.end();

  Smart::StatusCode result;

  std::unique_lock<decltype(mutex)> ul(mutex);

  if (running == false) {
    //
    // not yet activated
    //
    result = Smart::SMART_NOTACTIVATED;
  } else {
    //
    // state service is running
    //

    //
    // first check whether substate is valid
    //
    for (iterator = stateList.begin(); iterator != stateList.end(); ++iterator) {
      if (iterator->name == substate) {
        sIterator = iterator;
      }
    }
    if (sIterator != stateList.end()) {
      //
      // substate is valid ...
      //
      if (desiredState.action == SSA_CHANGE_STATE) {
        //
        // state change requested, therefore do not acquire substate until the
        // requested state change has been processed
        //
        //<alexej date="2010-09-08">
        //mutex.release(); // not needed any more

        // IMPORTANT:
        //
        // if state change is requested but required substate is already active, one has
        // to wait until the state change has been performed. Since the substate has been
        // active before the mainstate changed, one would wait here until the next activation
        // of this substate since normally only the newly activated substates are signalled.
        // To avoid this problem, we signal all active substates after a main state change
        // and can therefore safely wait here until the current main state change has been
        // performed.

        //sIterator->cond.wait(); // changed usage
        //<alexej date="2011-03-30">
        // cond->wait is ONLY necessary if the action for the current substate is either ACTIVATE or DEACTIVATE (resp. not NONE)
        // OR if the substate->action is NONE and the substate->state was not activated before the mainstate-change
        if (sIterator->action != STATE_ACTION_NONE || sIterator->state != STATE_ACTIVATED) {
          sIterator->cond->wait(ul);
        }
        //</alexej>

        //mutex.acquire(); // not needed eny more
        //</alexej>
        if (currentState == "Shutdown") {
          result = Smart::SMART_NOTACTIVATED;
        } else {
          //
          // state change completed since we got signal
          //
          (sIterator->cnt)++;

          result = Smart::SMART_OK;
        }
      } else {
        //
        // no state change in progress
        //
        if (sIterator->state == STATE_ACTIVATED) {
          //
          // requested substate is already active
          //
          (sIterator->cnt)++;

          result = Smart::SMART_OK;
        } else {
          //
          // requested substate not active => wait
          //
          //<alexej date="2010-09-08">
          //mutex.release(); // not needed to release any more

          (sIterator->cnt)++; // changed order (must be incremented before cond->wait!!!)
          if (currentState != "Shutdown") {
            sIterator->cond->wait(ul); // changed usage
          }

          if (currentState == "Shutdown") {
            (sIterator->cnt)--;
            result = Smart::SMART_NOTACTIVATED;
          } else {

            //mutex.acquire(); // not needed to acquire any more

            //(sIterator->cnt)++; // changed order (see above)
            //</alexej>
            result = Smart::SMART_OK;
          }
        }
      }
    } else {
      //
      // unknown substate
      //
      result = Smart::SMART_UNKNOWNSTATE;
    }
  }

  return result;
}

Smart::StatusCode StateSlave::tryAcquire(const std::string &substate) throw() {
  std::list<SmartSubStateEntry>::iterator iterator;
  std::list<SmartSubStateEntry>::iterator sIterator = stateList.end();

  Smart::StatusCode result;

  std::unique_lock<decltype(mutex)> ul(mutex);

  if (running == false) {
    //
    // not yet activated
    //
    result = Smart::SMART_NOTACTIVATED;
  } else {
    //
    // state service is running
    //

    //
    // first check whether substate is valid
    //
    for (iterator = stateList.begin(); iterator != stateList.end(); ++iterator) {
      if (iterator->name == substate) {
        sIterator = iterator;
      }
    }
    if (sIterator != stateList.end()) {
      //
      // substate is valid ...
      //
      if (desiredState.action == SSA_CHANGE_STATE) {
        //
        // state change requested, therefore do not acquire substate until the
        // requested state change has been processed
        //
        //mutex.release();

        // IMPORTANT:
        //
        // if state change is requested but required substate is already active, one has
        // to wait until the state change has been performed. Since the substate has been
        // active before the mainstate changed, one would wait here until the next activation
        // of this substate since normally only the newly activated substates are signalled.
        // To avoid this problem, we signal all active substates after a main state change
        // and can therefore safely wait here until the current main state change has been
        // performed.

        //sIterator->cond.wait();

        //mutex.acquire();

        //
        // state change completed since we got signal
        //
        //(sIterator->cnt)++;

        if (sIterator->action == STATE_ACTION_NONE && sIterator->state == STATE_ACTIVATED) {
          //<alexej date="2011-03-30">
          // we can safely count up, because current substate is not affected by the current mainstate-change AND the substate was active before this mainstate-change
          (sIterator->cnt)++;
          result = Smart::SMART_OK;
          //</alexej>
        } else {
          result = Smart::SMART_NOTACTIVATED;
        }
      } else {
        //
        // no state change in progress
        //
        if (sIterator->state == STATE_ACTIVATED) {
          //
          // requested substate is active => lock this substate
          //
          (sIterator->cnt)++;

          result = Smart::SMART_OK;
        } else {
          //
          // requested substate not active => return immediately
          //
          result = Smart::SMART_NOTACTIVATED;
        }
      }
    } else {
      //
      // unknown substate
      //
      result = Smart::SMART_UNKNOWNSTATE;
    }
  }

  return result;
}
// Acquire specified substate if available, otherwise return immediately.
// Smart::SMART_OK                  : everything is ok
// Smart::SMART_UNKNOWNSTATE        : returns immediately if the requested state is unknown --OK
// Smart::SMART_NOTACTIVATED        : state object of component not yet activated --OK
// Smart::SMART_ERROR_COMMUNICATION : communication problems
// Smart::SMART_ERROR               : something went wrong
//
// (hint: tryAcquire(nonneutral) can be used e.g. in a query handler to decide whether
//        to process the query or to immediately return an appropriate status code
//        indicating the components inactivity.)


Smart::StatusCode StateSlave::release(const std::string &substate) throw() {
  std::list<SmartSubStateEntry>::iterator iterator;
  std::list<SmartSubStateEntry>::iterator sIterator = stateList.end();

  Smart::StatusCode result;

  std::unique_lock<decltype(mutex)> ul(mutex);

  if (running == false) {
    //
    // not yet activated
    //
    result = Smart::SMART_NOTACTIVATED;
  } else {
    //
    // state service is running
    //

    //
    // first check whether substate is valid
    //
    for (iterator = stateList.begin(); iterator != stateList.end(); ++iterator) {
      if (iterator->name == substate) {
        sIterator = iterator;
      }
    }
    if (sIterator != stateList.end()) {
      //
      // substate is valid ... now check whether the substate is active
      //
      if (sIterator->state == STATE_ACTIVATED) {
        //
        // state is active
        //
        if (sIterator->cnt > 0) {
          //
          // release the substate
          //
          (sIterator->cnt)--;

          result = Smart::SMART_OK;
        } else {
          //
          // do nothing since state can not be released any more
          // occurs if you have more releases than acquires
          //

          result = Smart::SMART_OK;
        }
      } else {
        //
        // not activated, you never should release a deactivated state,
        // but this is not critical, simply do nothing
        //

        result = Smart::SMART_OK;
      }
    } else {
      //
      // substate is not valid
      //

      result = Smart::SMART_UNKNOWNSTATE;
    }
  }

  //
  // the following call to updateState is necessary since a state change request
  // might still be pending and can now be processed due the release of the last
  // locked substate
  //
  if (result == Smart::SMART_OK) {
    this->updateState();
  }

  return result;
}

}
}

