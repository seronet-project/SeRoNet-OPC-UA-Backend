/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/


#include <iostream>

#include "StateMaster.hpp"
#include "CommObjsSelfDescription.hpp"

namespace SeRoNet {
namespace State {

StateMaster::StateMaster(void)
    : state_proxy(NULL) {
  std::cerr << "CommPattern (state): ERROR: Entered default constructor StateMaster" << std::endl;

  throw (Exceptions::SeRoNetSDKException("CommPattern (stateClient): ERROR: Entered default constructor StateMaster"));
}

StateMaster::StateMaster(Utils::Component *m)
    : state_proxy(m) {
  // set the configuration flags approriately
  statusConnected = 0;
  portname = "";
  component = m;
}

StateMaster::~StateMaster(void) throw() {

}

Smart::StatusCode StateMaster::blocking(const bool b) {
  Smart::StatusCode result = Smart::SMART_OK;

  state_proxy.blocking(b);

  return result;
}

Smart::StatusCode StateMaster::setWaitState(const std::string &mainstate,
                                            const std::string &server,
                                            const std::string &service) {
  Smart::StatusCode status = Smart::SMART_ERROR;

  CommStateRequest request;
  CommStateResponse response;

  std::unique_lock<decltype(mutexConnection)> ul(mutexConnection);

  status = state_proxy.connect(server, service);

  if (status == Smart::SMART_OK) {
    // set request comm-obj
    request.setCommand(StateCommand::STATE_CMD_SET_STATE);
    request.setStateName(mainstate);

    // perform the query
    status = state_proxy.query(request, response);

    // get the reply state
    if (status == Smart::SMART_OK) {
      status = static_cast<Smart::StatusCode>(response.getStatus());
    }
    state_proxy.disconnect();
  }

  return status;
}

Smart::StatusCode StateMaster::getCurrentMainState(std::string &mainstate,
                                                   const std::string &server,
                                                   const std::string &service) {
  Smart::StatusCode status = Smart::SMART_ERROR;

  CommStateRequest request;
  CommStateResponse response;

  std::unique_lock<decltype(mutexConnection)> ul(mutexConnection);

  status = state_proxy.connect(server, service);

  if (status == Smart::SMART_OK) {
    // set request comm-obj
    request.setCommand(StateCommand::STATE_CMD_GET_CURRENT_STATE);

    // perform the query
    status = state_proxy.query(request, response);

    // get the reply state
    if (status == Smart::SMART_OK) {
      // get remote status
      status = static_cast<Smart::StatusCode>(response.getStatus());

      // copy received list back into mainstates-list
      if (status == Smart::SMART_OK) {
        mainstate = response.getStateList().back();
      } else {
        mainstate.clear();
      }
    }
    state_proxy.disconnect();
  }

  return status;
}

Smart::StatusCode StateMaster::getAllMainStates(std::list<std::string> &mainstates,
                                                const std::string &server,
                                                const std::string &service) {
  Smart::StatusCode status = Smart::SMART_ERROR;

  CommStateRequest request;
  CommStateResponse response;

  std::unique_lock<decltype(mutexConnection)> ul(mutexConnection);

  status = state_proxy.connect(server, service);

  if (status == Smart::SMART_OK) {
    // set request comm-obj
    request.setCommand(StateCommand::STATE_CMD_GET_MAIN_STATES);

    // perform the query
    status = state_proxy.query(request, response);

    // get the reply state
    if (status == Smart::SMART_OK) {
      // get remote status
      status = static_cast<Smart::StatusCode>(response.getStatus());

      // copy received list back into mainstates-list
      if (status == Smart::SMART_OK) {
        std::vector<std::string> stateList = response.getStateList();
        mainstates = std::list<std::string>(stateList.begin(), stateList.end());
      } else {
        mainstates.clear();
      }
    }
    state_proxy.disconnect();
  }

  return status;
}

Smart::StatusCode StateMaster::getSubStates(const std::string &mainstate,
                                            std::list<std::string> &substates,
                                            const std::string &server,
                                            const std::string &service) {
  Smart::StatusCode status = Smart::SMART_ERROR;

  CommStateRequest request;
  CommStateResponse response;

  std::unique_lock<decltype(mutexConnection)> ul(mutexConnection);

  status = state_proxy.connect(server, service);

  if (status == Smart::SMART_OK) {
    // set request comm-obj
    request.setCommand(StateCommand::STATE_CMD_GET_SUB_STATES);
    request.setStateName(mainstate);

    // perform the query
    status = state_proxy.query(request, response);

    // get the reply state
    if (status == Smart::SMART_OK) {
      // get remote status
      status = static_cast<Smart::StatusCode>(response.getStatus());

      // copy received list back into substates-list
      if (status == Smart::SMART_OK) {
        std::vector<std::string> stateList = response.getStateList();
        substates = std::list<std::string>(stateList.begin(), stateList.end());
      } else {
        substates.clear();
      }
    }
    state_proxy.disconnect();
  }

  return status;
}

}
}
