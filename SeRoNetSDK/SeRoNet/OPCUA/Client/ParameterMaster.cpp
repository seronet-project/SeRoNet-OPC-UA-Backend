/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#include "ParameterMaster.hpp"

#include "../../DefaultCommObjects/Description/ParameterRequestDescription.hpp"
#include "../../DefaultCommObjects/Description/ParameterResponseDescription.hpp"

const std::string SMART_NONS = "NoNamingService"; //FIXME check why this const is needed


SeRoNet::OPCUA::Client::ParameterMaster::ParameterMaster(Utils::Component *component)
    : mutex(), component(component), query_client(component) {

  _use_ns = component->getName().compare(SMART_NONS) == 0 ? false : true;
}

Smart::StatusCode SeRoNet::OPCUA::Client::ParameterMaster::sendParameterWait(const SeRoNet::DefaultCommObjects::CommParameterRequest &request,
                                                                             SeRoNet::DefaultCommObjects::CommParameterResponse &response,
                                                                             const std::string &comp_name,
                                                                             const std::string &service_name) {
  std::unique_lock<decltype(mutex)> lock(mutex);

  Smart::StatusCode status;

  // connect manually since connect with constructor is retrying

  if (_use_ns) {
    status = query_client.connect(comp_name, service_name);
  } else {
    status = query_client.connect(SMART_NONS, service_name);
  }

  switch (status) {
    case Smart::SMART_OK:
      // everything ok
      break;
    case Smart::SMART_SERVICEUNAVAILABLE:
      // the comp_name is unknown or does not provide a Parameter slave
      return Smart::SMART_UNKNOWNCOMPONENT;
    case Smart::SMART_INCOMPATIBLESERVICE:
      // the "param" service of the comp_name is not compatible to the master
      return Smart::SMART_UNKNOWNCOMPONENT;
    case Smart::SMART_ERROR_COMMUNICATION:
      // problems while connecting to the slave
      return Smart::SMART_ERROR_COMMUNICATION;
    default:
      // problems while connecting to the slave
      return Smart::SMART_ERROR;
  }

  status = query_client.query(request, response);

  query_client.disconnect();

  return status;
}
