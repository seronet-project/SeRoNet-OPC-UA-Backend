/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#pragma once

#include "IDescription.hpp"

namespace SeRoNet {
namespace CommunicationObjects {
namespace Description {
template <typename T>
IDescription::shp_t SelfDescription(T *t, std::string name = "");


template <typename T>
IDescription::shp_t SelfDescription(T */*t*/, std::string /*name*/)
{
  static_assert(sizeof(T) == 0, "No Specialization for Template type provided.");
  return nullptr;
}

}
}
}
