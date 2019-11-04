/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/


#pragma once

#include <cstdint>
#include <type_traits>
#include <string>
#include <sstream>

namespace ValueCreator
{
  /*template <typename T>
  T createValue(int i)
  {
    static_assert(sizeof(T) == 0, "No Specialization not implemented.");
    return nullptr;
  }*/

  /// All number can be converted by the compiler
  template <typename T, typename = std::enable_if<std::is_arithmetic<T>::value, int>>
  T createValue(int i)
  {
    return i;
  }

  template <>
  std::string createValue(int i)
  {
    std::stringstream ss;
    ss << "i: " <<i;
    return ss.str();
  }
}
