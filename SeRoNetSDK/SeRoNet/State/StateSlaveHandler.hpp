///
/// \file StateSlaveHandler.hpp
/// \author Christian von Arnim
/// \date 30.07.2018
///

#pragma once

namespace SeRoNet {
namespace State {
class StateSlaveHandler;
}
}

#include "StateSlave.hpp"

#include "../../../SmartSoftComponentDeveloperAPIcpp/SmartSoft_CD_API/smartQueryStatus.h"
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
  typedef int QueryId;
  /** Constructor.
   *
   * @param slave  <I>StateSlave</I> needed to access it from the handler
   */
  StateSlaveHandler(StateSlave *state) throw();

  /// Destructor
  virtual ~StateSlaveHandler() throw();

  virtual void handleQuery(const QueryId &id, const CommStateRequest &request) throw();
};

}
}
