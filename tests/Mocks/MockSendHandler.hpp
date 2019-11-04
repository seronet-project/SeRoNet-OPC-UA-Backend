/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#pragma once

#include <gmock/gmock.h>
#include <SeRoNet/OPCUA/Server/SendServerHandler.hpp>

template<typename DataType>
class MockSendHandler
    : public SeRoNet::OPCUA::Server::SendServerHandler<DataType> {
 public:
  using SeRoNet::OPCUA::Server::SendServerHandler<DataType>::SendServerHandler;
  // Make public availiable
  MOCK_METHOD1_T(handleSend, void (const DataType&));
};
