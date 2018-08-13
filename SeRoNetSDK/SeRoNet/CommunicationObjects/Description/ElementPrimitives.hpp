///
/// \file ElementPrimitives.hpp
/// \author Christian von Arnim
/// \date 15.02.2018
///

#pragma once

#include "ElementPrimitive.hpp"
#include <cstdint>
#include <string>

#include "SelfDescriptionPrimitives.hpp"

namespace SeRoNet {
namespace CommunicationObjects {
namespace Description {
using ElementBool = ElementPrimitive<bool>;
using ElementInt32 = ElementPrimitive<std::int32_t>;

using ElementFloat = ElementPrimitive<float>;
using ElementDouble = ElementPrimitive<double>;

using ElementString = ElementPrimitive<std::string>;

}
}
}
