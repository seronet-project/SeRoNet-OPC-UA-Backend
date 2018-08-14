//
// Created by ac120371 on 10.08.2018.
//

#include "ParameterSlave.hpp"
#include "QueryServer.hpp"
#include "../../CommunicationObjects/Description/DefaultObjects/ParameterRequestDescription.hpp"
#include "../../CommunicationObjects/Description/DefaultObjects/ParameterResponseDescription.hpp"

namespace SeRoNet {
namespace OPCUA {
namespace Server {

ParameterSlave::ParameterQueryHandler::ParameterQueryHandler(QueryServer<SeRoNet::CommunicationObjects::DefaultObjects::CommParameterRequest,
                                                                         SeRoNet::CommunicationObjects::DefaultObjects::CommParameterResponse> *server,
                                                             ParameterUpdateHandler *param_handler) noexcept
    : param_handler(param_handler),
      QueryServerHandler<CommunicationObjects::DefaultObjects::CommParameterRequest,
                         CommunicationObjects::DefaultObjects::CommParameterResponse>(server) {

}
void ParameterSlave::ParameterQueryHandler::handleQuery(const int &id,
                                                        const SeRoNet::CommunicationObjects::DefaultObjects::CommParameterRequest &request) noexcept {

  SeRoNet::CommunicationObjects::DefaultObjects::CommParameterResponse answer;

  answer = param_handler->handleParameter(request);

  this->server->answer(id, answer);
}

ParameterSlave::ParameterSlave(Smart::IComponent *comp,
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