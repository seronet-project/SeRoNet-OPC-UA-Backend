///
/// \file ElementPrimitives.hpp
/// \author Christian von Arnim
/// \date 15.02.2018
///

#pragma once

#include "ElementPrimitive.hpp"
#include <cstdint>
#include <string>

namespace SeRoNet {
namespace CommunicationObjects {
namespace Description {
using ElementInt32 = ElementPrimitive<std::int32_t>;
using ElementString = ElementPrimitive<std::string>;

}
}
}
