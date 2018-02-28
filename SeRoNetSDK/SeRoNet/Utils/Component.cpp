///
/// \file Component.cpp
/// \author Sebastian Friedl
/// \date 26.02.2018
///

#include <open62541/open62541.h>
#include "Component.hpp"

namespace SeRoNet {
namespace Utils {

Component::Component(const std::string &componentName)
    : IComponent(componentName) {
  config = UA_ServerConfig_new_default();
  config->customHostname = UA_STRING_ALLOC(componentName.c_str());
  config->mdnsServerName = UA_STRING_ALLOC(componentName.c_str());
  config->applicationDescription.applicationName = UA_LOCALIZEDTEXT_ALLOC("", componentName.c_str());
  server = UA_Server_new(config);
  m_nsIndex1 = UA_Server_addNamespace(server, "http://seronet-projekt.de/");
};

Smart::StatusCode Component::run() {
  UA_Server_run(server, &running);
}

Smart::StatusCode Component::blocking(const bool b) {
  return Smart::StatusCode::SMART_OK;
}

} //  namespace Utils
} //  namespace SeRoNet