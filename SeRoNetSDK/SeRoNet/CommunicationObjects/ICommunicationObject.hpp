///
/// \file ICommunicationObject.hpp
/// \author Christian von Arnim
/// \date 15.02.2018
///

#pragma once

#include "Description/ComplexType.hpp"

namespace SeRoNet {
namespace CommunicationObjects {
class ICommunicationObject {
 public:
  virtual Description::ComplexType::shp_t getObjectDescription(std::string name) = 0;
};
}
}
