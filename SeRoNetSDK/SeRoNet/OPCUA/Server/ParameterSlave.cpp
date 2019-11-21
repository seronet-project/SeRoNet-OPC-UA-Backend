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

ParameterSlave::ParameterQueryHandler::ParameterQueryHandler(ParameterUpdateHandler *param_handler) noexcept
    : param_handler(param_handler)
{
}
void ParameterSlave::ParameterQueryHandler::handleQuery(IQueryServer& server, const Smart::QueryIdPtr &id,
                                                        const DefaultCommObjects::CommParameterRequest &request) noexcept {

  SeRoNet::DefaultCommObjects::CommParameterResponse answer;

  answer = param_handler->handleParameter(request);

  server.answer(id, answer);
}

ParameterSlave::ParameterSlave(Utils::Component *comp,
                               ParameterUpdateHandler *hnd,
                               std::string slave_address)
    : component(comp)
    , query_handler(std::make_shared<ParameterQueryHandler>(hnd))
    , thread_handler(std::make_shared<SeRoNet::Utils::HsUlm::ThreadQueueQueryHandler<SeRoNet::DefaultCommObjects::CommParameterRequest,
                                                                                     SeRoNet::DefaultCommObjects::CommParameterResponse>>(comp,query_handler))
    , query_server(comp, slave_address, thread_handler)
{

}

}
}
}
