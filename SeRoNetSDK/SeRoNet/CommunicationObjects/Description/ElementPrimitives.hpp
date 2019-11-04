/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#pragma once

#include "ElementPrimitive.hpp"
#include <cstdint>
#include <string>

#include "SelfDescriptionPrimitives.hpp"

namespace SeRoNet {
namespace CommunicationObjects {
namespace Description {
using ElementBool = ElementPrimitive<bool>;
using ElementInt8 = ElementPrimitive<std::int8_t>;
using ElementUInt8 = ElementPrimitive<std::uint8_t>;
using ElementInt16 = ElementPrimitive<std::int16_t>;
using ElementUInt16 = ElementPrimitive<std::uint16_t>;
using ElementInt32 = ElementPrimitive<std::int32_t>;
using ElementUInt32 = ElementPrimitive<std::uint32_t>;
using ElementInt64 = ElementPrimitive<std::int64_t>;
using ElementUInt64 = ElementPrimitive<std::uint64_t>;


using ElementFloat = ElementPrimitive<float>;
using ElementDouble = ElementPrimitive<double>;

using ElementString = ElementPrimitive<std::string>;

}
}
}
