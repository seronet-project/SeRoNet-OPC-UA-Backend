///
/// \file SelfDescription.hpp
/// \author Christian von Arnim
/// \date 23.04.2018
///

#pragma once

#include "ComplexType.hpp"

namespace SeRoNet {
namespace CommunicationObjects {
namespace Description {

template <typename T>
IDescription::shp_t SelfDescription(T *t, std::string name = "");


template <typename T>
IDescription::shp_t SelfDescription(T *t, std::string name)
{
  static_assert(sizeof(T) == 0, "No Specialization for Template type provided.");
  return nullptr;
}

}
}
}
