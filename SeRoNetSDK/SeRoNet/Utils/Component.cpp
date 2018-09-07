///
/// \file Component.cpp
/// \author Sebastian Friedl
/// \date 26.02.2018
///

#include <open62541/open62541.h>
#include "Component.hpp"
#include "../Exceptions/NotImplementedException.hpp"
#include "../OPCUA/Server/OpcuaServer.hpp"

namespace SeRoNet {
namespace Utils {

Component::Component(const std::string &componentName)
    : IComponent(componentName) {
  SeRoNet::OPCUA::Server::OpcUaServer::instance().initServer(componentName);
};

Component::Component(const std::string &componentName, int /*argc*/, char **/*argv*/) : Component(componentName) {}

Smart::StatusCode Component::run() {
  SeRoNet::OPCUA::Server::OpcUaServer::instance().run();
  return Smart::StatusCode::SMART_OK;
}

Smart::StatusCode Component::blocking(const bool /*b*/) {
  return Smart::StatusCode::SMART_OK;
}
Smart::ITimerManager *Component::getTimerManager() {
  throw SeRoNet::Exceptions::NotImplementedException(__FUNCTION__);
}

} //  namespace Utils
} //  namespace SeRoNet