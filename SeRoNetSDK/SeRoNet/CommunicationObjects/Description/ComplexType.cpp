/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/


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