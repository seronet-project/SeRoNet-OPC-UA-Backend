/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/


#pragma once

#include "SelfDescription.hpp"
#include "ElementArray.hpp"
#include <vector>

namespace SeRoNet {
namespace CommunicationObjects {
namespace Description {

template<typename TYPE>
IDescription::shp_t SelfDescription(std::vector<TYPE> *ptr, std::string name) {
  auto ret = std::make_shared<ArrayOf<TYPE>>(name,
                                             [ptr]() { return *ptr; },
                                             [ptr](const std::vector<TYPE> &newEl) { *ptr = newEl; });
  return ret;
}

}
}
}
