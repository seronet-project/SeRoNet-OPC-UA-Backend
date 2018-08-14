///
/// \file StateSlaveHandler.cpp
/// \author Christian von Arnim
/// \date 30.07.2018
///

#include "StateSlaveHandler.hpp"

namespace SeRoNet {
namespace State {

StateSlaveHandler::StateSlaveHandler(StateSlave *slave) throw()
    : stateSlave(slave), QueryServerHandler<CommStateRequest, CommStateResponse>(slave->query_server) {}

StateSlaveHandler::~StateSlaveHandler() throw() {}

void StateSlaveHandler::handleQuery(const QueryId &id, const CommStateRequest &request) throw() {
  CommStateResponse reply;
  std::vector<std::string> state_list;
  std::string mainstate;
  Smart::StatusCode ret_val;

  // call appropriate command depending on command-id
  switch (request.getCommand()) {
    case STATE_CMD_SET_STATE:
      // get main-state
      mainstate = request.getStateName();

      // call handler for setMainState
      StateSlave::hndSetMainState(stateSlave,
                                  dynamic_cast<SeRoNet::OPCUA::Server::QueryServer<CommStateRequest,
                                                                                   CommStateResponse> *>(server),
                                  id,
                                  mainstate);
      break;

    case STATE_CMD_GET_CURRENT_STATE:ret_val = StateSlave::hndGetCurrentState((void *) stateSlave, mainstate);

      state_list.clear();
      state_list.push_back(mainstate);

      reply.setStateList(state_list);
      reply.setStatus(static_cast<int>(ret_val));

      // reply to client
      server->answer(id, reply);
      break;

    case STATE_CMD_GET_MAIN_STATES:
      //get state-list
      ret_val = StateSlave::hndGetMainStates((void *) stateSlave, state_list);

      // safe state list in reply comm-obj
      reply.setStateList(state_list);
      reply.setStatus(static_cast<int>(ret_val));

      // reply to client
      server->answer(id, reply);
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
      server->answer(id, reply);
      break;

    default:std::cout << "<StateSlaveHandler> unknown command\n" << std::endl;
      break;
  };

}

}
}
