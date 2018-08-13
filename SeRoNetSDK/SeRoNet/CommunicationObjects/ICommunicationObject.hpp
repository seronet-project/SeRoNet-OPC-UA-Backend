///
/// \file ICommunicationObject.hpp
/// \author Christian von Arnim
/// \date 15.02.2018
///

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
