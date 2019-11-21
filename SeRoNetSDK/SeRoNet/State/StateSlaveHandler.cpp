/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#include "StateSlaveHandler.hpp"

namespace SeRoNet {
namespace State {

StateSlaveHandler::StateSlaveHandler(StateSlave *slave)
    : stateSlave(slave) {}

StateSlaveHandler::~StateSlaveHandler() {}

void StateSlaveHandler::handleQuery(IQueryServer &server, const Smart::QueryIdPtr &id, const CommStateRequest &request) {
  CommStateResponse reply;
  std::vector<std::string> state_list;
  std::string mainstate;
  Smart::StatusCode ret_val;
  auto server_ptr = dynamic_cast<OPCUA::Server::QueryServer<CommStateRequest, CommStateResponse>*>(&server);

  // call appropriate command depending on command-id
  switch (request.getCommand()) {
    case STATE_CMD_SET_STATE:
      // get main-state
      mainstate = request.getStateName();

      // call handler for setMainState
      StateSlave::hndSetMainState(stateSlave,
                                  server_ptr,
                                  id,
                                  mainstate);
      break;

    case STATE_CMD_GET_CURRENT_STATE:ret_val = StateSlave::hndGetCurrentState((void *) stateSlave, mainstate);

      state_list.clear();
      state_list.push_back(mainstate);

      reply.setStateList(state_list);
      reply.setStatus(static_cast<int>(ret_val));

      // reply to client
      server.answer(id, reply);
      break;

    case STATE_CMD_GET_MAIN_STATES:
      //get state-list
      ret_val = StateSlave::hndGetMainStates((void *) stateSlave, state_list);

      // safe state list in reply comm-obj
      reply.setStateList(state_list);
      reply.setStatus(static_cast<int>(ret_val));

      // reply to client
      server.answer(id, reply);
      break;

    case STATE_CMD_GET_SUB_STATES:
      // get main state
      mainstate = request.getStateName();
      //get state-list
      ret_val = StateSlave::hndGetSubStates((void *) stateSlave, mainstate, state_list);

      // safe state list in reply comm-obj
      reply.setStateList(state_list);
      reply.setStatus(static_cast<int>(ret_val));

      // reply to client
      server.answer(id, reply);
      break;

    default:std::cout << "<StateSlaveHandler> unknown command\n" << std::endl;
      break;
  };

}

}
}
