//
// Created by ac120371 on 23.08.2018.
//

#include "WiringSlave.hpp"
#include "../../CommunicationObjects/Description/DefaultObjects/WiringCommObjectDescription.hpp"

namespace SeRoNet {
namespace OPCUA {
namespace Server {

WiringQueryHandler::WiringQueryHandler(WiringSlave *slave) noexcept
    : QueryServerHandler<CommunicationObjects::DefaultObjects::WiringCommObject,
                         CommunicationObjects::DefaultObjects::WiringCommObject>(slave->wiring),
      wiringSlave(slave) {}

void WiringQueryHandler::handleQuery(const int &id,
                                     const CommunicationObjects::DefaultObjects::WiringCommObject &request) {
  CommunicationObjects::DefaultObjects::WiringCommObject answer;

  answer = wiringSlave->handleWiring(request);

  this->server->answer(id, answer);
}

WiringSlave::WiringSlave(Smart::IComponent *component, std::string slaveaddress)
    : component(component) {
  {

    const std::string SMART_NONS = "NoNamingService"; //FIXME check why this const is needed

    // added functionality to work without NamingService
    if (component->getName() == SMART_NONS) {
      if (slaveaddress.empty()) {
        wiring = nullptr;
        std::cout
            << "<WiringSlave> Warning: wiring slave is not initialized properly. "
            << "Please use second parameter of WiringSlave-constuctor!"
            << std::endl;
      } else {
        // Slaveaddress has to contain a propper "ip:portnr" string for WiringSlave.
        // This "ip:portnr" will be used by WiringMaster to connect to WiringSlave!
        wiring = new QueryServer<CommunicationObjects::DefaultObjects::WiringCommObject,
                                 CommunicationObjects::DefaultObjects::WiringCommObject>(component, slaveaddress);
      }
    } else {
      wiring = new QueryServer<CommunicationObjects::DefaultObjects::WiringCommObject,
                               CommunicationObjects::DefaultObjects::WiringCommObject>(component, "wiring");
    }

    handler = new WiringQueryHandler(this);

    // Handling is done in separate thread, otherwise blocking of main thread occures!
    threadHandler =
        new SeRoNet::Utils::HsUlm::ThreadQueueQueryHandler<CommunicationObjects::DefaultObjects::WiringCommObject,
                                                           CommunicationObjects::DefaultObjects::WiringCommObject>(
            component,
            handler);
  }
}

CommunicationObjects::DefaultObjects::WiringCommObject WiringSlave::handleWiring(const CommunicationObjects::DefaultObjects::WiringCommObject &request) {
  CommunicationObjects::DefaultObjects::WiringCommObject answer;
  Smart::StatusCode status;
  std::string command, portName, servername, servicename;

  request.getCommand(command, portName, servername, servicename);

  std::unique_lock<decltype(mutex)> lock(mutex);

  if (command == "connect") {
    //
    // look for the portName
    //
    auto it = ports.find(portName);

    if (it != ports.end()) {
      //
      // portName known, now call the connect method of the client object
      //
      status = it->second.connectFunction(servername, servicename);

      answer.setStatus(status);
    } else {
      answer.setStatus(Smart::SMART_UNKNOWNPORT);
    }
  } else if (command == "disconnect") {
    //
    // look for the portName
    //
    auto it = ports.find(portName);

    if (it != ports.end()) {
      //
      //
      // portName known, now call the disconnect method of the client object
      //
      status = it->second.disconnectFunction();

      answer.setStatus(status);
    } else {
      answer.setStatus(Smart::SMART_UNKNOWNPORT);
    }
  } else {
    answer.setStatus(Smart::SMART_ERROR);
  }

  return answer;
}

Smart::StatusCode WiringSlave::add(const std::string &portName,
                                   std::function<Smart::StatusCode(const std::string &,
                                                                   const std::string &)> connectFunction,
                                   std::function<Smart::StatusCode()> disconnectFunction) {
  if (wiring == nullptr) {
    std::cout
        << "<WiringSlave> Warning: wiring slave is not initialized properly. "
        << "Please use second parameter of WiringSlave-constuctor!"
        << std::endl;
  }

  std::unique_lock<decltype(mutex)> lock(mutex);
  PortElement tmp{portName, connectFunction, disconnectFunction};
  auto ret = ports.insert(std::pair<std::string, PortElement>(portName, tmp));
  return ret.second ? Smart::SMART_OK : Smart::SMART_PORTALREADYUSED;
}

Smart::StatusCode WiringSlave::remove(const std::string &portName) {
  std::unique_lock<decltype(mutex)> lock(mutex);
  auto ret = ports.erase(portName);
  return ret == 1 ? Smart::SMART_OK : Smart::SMART_PORTALREADYUSED; // remove was successful if one element was removed
}

WiringSlave::~WiringSlave() {
  std::unique_lock<decltype(mutex)> lock(mutex);
  delete handler;
  delete threadHandler;
}

bool WiringSlave::PortElement::operator==(const WiringSlave::PortElement &rhs) const {
  return portName == rhs.portName;
}

bool WiringSlave::PortElement::operator!=(const WiringSlave::PortElement &rhs) const {
  return !(rhs == *this);
}

} // end namespace Server
} // end namespace OPCUA
} // end namespace SeRoNet
