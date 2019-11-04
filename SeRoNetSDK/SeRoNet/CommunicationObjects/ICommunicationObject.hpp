/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/


#pragma once

#include "Description/ComplexType.hpp"

namespace SeRoNet {
namespace CommunicationObjects {

/*
 * TODO @xfl check if get ObjectDescription is needed any more
 */
class ICommunicationObject {
 public:
  virtual Description::ComplexType::shp_t getObjectDescription(std::string name) = 0;
};
}
}
