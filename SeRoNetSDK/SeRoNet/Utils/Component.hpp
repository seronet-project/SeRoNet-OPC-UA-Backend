/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#pragma once

#include "../../Definitions.hpp"
#include "../OPCUA/Server/OpcuaServer.hpp"

#include <smartIComponent.h>
#include <smartStatusCode.h>

namespace SeRoNet {
namespace Utils {
class SERONETSDK_EXPORT Component :
    public Smart::IComponent {

 private:
  UA_Boolean running = true;
  std::shared_ptr<SeRoNet::OPCUA::Server::OpcUaServer> m_pOpcUaServer;
 public:
  explicit Component(const std::string &componentName);
  Component(const std::string &componentName, int argc, char *argv[]);

  Smart::StatusCode run() override;

  Smart::StatusCode blocking(bool b) override;
  Smart::ITimerManager *getTimerManager() override;

  std::shared_ptr<SeRoNet::OPCUA::Server::OpcUaServer> getOpcUaServer();

  /// \TODO remove
  void stopRunning() {
    running = false;
    m_pOpcUaServer->stopRunning();
  }
};

}
}
