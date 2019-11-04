/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#pragma once

#include "SendServer.hpp"

#include <smartIInputHandler_T.h>
#include <smartISendServerPattern_T.h>
#include <smartICommunicationObject.h>

namespace SeRoNet {
namespace OPCUA {
namespace Server {

template<class DataType>
class SendServerHandler : public Smart::IInputHandler<DataType> {

 public:

  SendServerHandler() = default;

  /** Default constructor.
   *
   *  The constructor requires the server pointer to register itself
   *  for handling incoming send commands.
   *
   *  @param server the pointer to the ISendServerPattern instance
   *
   */
  SendServerHandler(SendServer<DataType>
                    *server)
      :
      Smart::IInputHandler<DataType>(server) {}

  /// Default destructor.
  ~SendServerHandler() = default;

  void handle_input(const DataType &input) override {
    this->handleSend(input);
  }

  /** Handler method for an incoming command.
   *
   *  This method is called by the ISendServerPattern every time
   *  a new data is received. It must be provided by the component
   *  developer to handle incoming data. Since the method is
   *  executed by the communication thread, it must be very fast
   *  and non-blocking.
   *
   *  Usually the data will be inserted into a queue and another
   *  working thread processes the command. The IActiveQueueInputHandlerDecorator
   *  provides such a processing pattern.
   *
   *  @param data communicated DataType object (Communication Object)
   */
  virtual void handleSend(const DataType &data) {
    return;
  };
};

}  // namespace Server
}  // namespace OPCUA
}  // namespace SeRoNet
