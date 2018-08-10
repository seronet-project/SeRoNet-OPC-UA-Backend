///
/// \file SelfDescriptionArray.hpp
/// \author Christian von Arnim
/// \date 09.08.2018
///

#pragma once

#include "SelfDescription.hpp"
#include "ElementArray.hpp"
#include <vector>

namespace SeRoNet {
namespace CommunicationObjects {
namespace Description {

template <typename TYPE>
IDescription::shp_t SelfDescription(std::vector<TYPE> *ptr, std::string name) {
  auto ret = std::make_shared<ArrayOf<TYPE>>(name, [ptr](){return *ptr;}, [ptr](const std::vector<TYPE> &newEl){*ptr = newEl;});
  return ret;
}

}
}
}
