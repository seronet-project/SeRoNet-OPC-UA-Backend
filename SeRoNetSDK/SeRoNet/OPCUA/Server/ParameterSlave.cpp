/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#include "ParameterSlave.hpp"
#include "QueryServer.hpp"
#include "../../DefaultCommObjects/Description/ParameterRequestDescription.hpp"
#include "../../DefaultCommObjects/Description/ParameterResponseDescription.hpp"

namespace SeRoNet {
namespace OPCUA {
namespace Server {

ParameterSlave::ParameterQueryHandler::ParameterQueryHandler(QueryServer<DefaultCommObjects::CommParameterRequest,
                                                                         DefaultCommObjects::CommParameterResponse> *server,
                                                             ParameterUpdateHandler *param_handler) noexcept
    : param_handler(param_handler),
      QueryServerHandler<DefaultCommObjects::CommParameterRequest,
                         DefaultCommObjects::CommParameterResponse>(server) {

}
void ParameterSlave::ParameterQueryHandler::handleQuery(const int &id,
                                                        const DefaultCommObjects::CommParameterRequest &request) noexcept {

  SeRoNet::DefaultCommObjects::CommParameterResponse answer;

  answer = param_handler->handleParameter(request);

  this->server->answer(id, answer);
}

ParameterSlave::ParameterSlave(Utils::Component *comp,
                               ParameterUpdateHandler *hnd,
                               std::string slave_address)
    : component(comp),
      query_server(comp, slave_address),
      query_handler(&query_server, hnd),
      thread_handler(comp, &query_handler) {

}

}
}
}