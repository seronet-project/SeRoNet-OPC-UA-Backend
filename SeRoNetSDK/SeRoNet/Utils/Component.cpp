///
/// \file Component.cpp
/// \author Sebastian Friedl
/// \date 26.02.2018
///

#include <open62541/open62541.h>
#include "Component.hpp"
#include "../Exceptions/NotImplementedException.hpp"

namespace SeRoNet {
namespace Utils {

Component::Component(const std::string &componentName)
    : IComponent(componentName) {
  config = UA_ServerConfig_new_default();
  config->mdnsServerName = UA_String_fromChars(componentName.c_str());
  config->applicationDescription.applicationName = UA_LOCALIZEDTEXT_ALLOC("", componentName.c_str());
  config->applicationDescription.applicationType = UA_APPLICATIONTYPE_SERVER;
  config->mdnsServerName = UA_String_fromChars(componentName.c_str());

  UA_String *caps = UA_String_new();
  *caps = UA_String_fromChars(componentName.c_str());
  config->serverCapabilities = caps;

  server = UA_Server_new(config);
  m_nsIndex1 = UA_Server_addNamespace(server, "http://seronet-projekt.de/");
};

Smart::StatusCode Component::run() {
  UA_Server_run(server, &running);
  return Smart::StatusCode::SMART_OK;
}

Smart::StatusCode Component::blocking(const bool b) {
  return Smart::StatusCode::SMART_OK;
}
Smart::ITimerManager *Component::getTimerManager() {
  throw SeRoNet::Exceptions::NotImplementedException(__FUNCTION__);
}

} //  namespace Utils
} //  namespace SeRoNet