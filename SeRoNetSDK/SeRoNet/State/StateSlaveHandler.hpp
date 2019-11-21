/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#pragma once

namespace SeRoNet {
namespace State {
class StateSlaveHandler;
}
}

#include "StateSlave.hpp"

#include <smartQueryStatus.h>
#include "../OPCUA/Server/QueryServerHandler.hpp"

namespace SeRoNet {

namespace State {

/** @internal
   *  Handler for slave part of wiring pattern.
   *
   *  The wiring handler is called by the internally used query pattern
   *  and connects / disconnects a port with a server.
   */
class StateSlaveHandler : public OPCUA::Server::QueryServerHandler<CommStateRequest, CommStateResponse> {
 private:
  /// used to access the StateSlave from the handler
  StateSlave *stateSlave;

 public:
  /** Constructor.
   *
   * @param slave  <I>StateSlave</I> needed to access it from the handler
   */
  StateSlaveHandler(StateSlave *state);

  /// Destructor
  virtual ~StateSlaveHandler();

  virtual void handleQuery(IQueryServer &server, const Smart::QueryIdPtr &id, const CommStateRequest &request);
};

}
}
