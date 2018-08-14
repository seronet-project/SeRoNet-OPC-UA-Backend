//
// Created by ac120371 on 09.08.2018.
//

#ifndef SERONETSDK_PARAMETERUPDATEHANDLER_HPP
#define SERONETSDK_PARAMETERUPDATEHANDLER_HPP

#include "../../CommunicationObjects/DefaultObjects/ParameterResponse.hpp"
#include "../../CommunicationObjects/DefaultObjects/ParameterRequest.hpp"

class ParameterSlave;

namespace SeRoNet {
namespace OPCUA {
namespace Server {

class ParameterUpdateHandler {
 public:
  ParameterUpdateHandler() = default;
  virtual ~ParameterUpdateHandler() = default;
  virtual SeRoNet::CommunicationObjects::DefaultObjects::CommParameterResponse handleParameter(const SeRoNet::CommunicationObjects::DefaultObjects::CommParameterRequest &request)=0;
};

}
}
}

#endif //SERONETSDK_PARAMETERUPDATEHANDLER_HPP
