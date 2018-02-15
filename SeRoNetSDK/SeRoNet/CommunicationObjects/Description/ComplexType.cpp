///
/// \file ComplexType.cpp
/// \author Christian von Arnim
/// \date 15.02.2018
///

#include "ComplexType.hpp"

namespace SeRoNet {
namespace CommunicationObjects {
namespace Description {

ComplexType &ComplexType::add(IDescription::shp_t element) {
  m_elements.push_back(element);
  return *this;
}

void ComplexType::accept(IVisitorDescription *visitor) {
  visitor->visit(this);
}
}
}
}