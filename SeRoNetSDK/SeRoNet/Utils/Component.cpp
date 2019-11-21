/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#include "Component.hpp"
#include "../Exceptions/NotImplementedException.hpp"
#include "../OPCUA/Server/OpcuaServer.hpp"

namespace SeRoNet {
namespace Utils {

Component::Component(const std::string &componentName)
    : IComponent(componentName), m_pOpcUaServer(std::make_shared<SeRoNet::OPCUA::Server::OpcUaServer>()){
  m_pOpcUaServer->initServer(componentName);
};

Component::Component(const std::string &componentName, int /*argc*/, char **/*argv*/) : Component(componentName) {}

Smart::StatusCode Component::run() {
  m_pOpcUaServer->run();
  return Smart::StatusCode::SMART_OK;
}

void Component::signal_shutdown() {
    running = false;
    m_pOpcUaServer->stopRunning();
}
Smart::StatusCode Component::blocking(const bool /*b*/) {
  return Smart::StatusCode::SMART_OK;
}
Smart::ITimerManager *Component::getTimerManager() {
  throw SeRoNet::Exceptions::NotImplementedException(__FUNCTION__);
}
std::shared_ptr<SeRoNet::OPCUA::Server::OpcUaServer> Component::getOpcUaServer() {
  return m_pOpcUaServer;
}

} //  namespace Utils
} //  namespace SeRoNet
