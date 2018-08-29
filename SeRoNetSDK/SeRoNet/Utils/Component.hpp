/**
 * @author Sebastian Friedl
 */
#pragma once

#include <open62541.h>
#include "../OPCUA/Server/OpcuaServer.hpp"

#include "../../../SmartSoftComponentDeveloperAPIcpp/SmartSoft_CD_API/smartIComponent.h"
#include "../../../SmartSoftComponentDeveloperAPIcpp/SmartSoft_CD_API/smartStatusCode.h"

namespace SeRoNet {
namespace Utils {
class Component :
    public Smart::IComponent {

 private:
  UA_Boolean running = true;
 public:
  explicit Component(const std::string &componentName);
  Component(const std::string &componentName, int argc, char *argv[]);

  Smart::StatusCode run() override;

  Smart::StatusCode blocking(bool b) override;
  Smart::ITimerManager *getTimerManager() override;

  /// \TODO remove
  void stopRunning() {
    running = false;
    SeRoNet::OPCUA::Server::OpcUaServer::instance().stopRunning();
  }
};

}
}
