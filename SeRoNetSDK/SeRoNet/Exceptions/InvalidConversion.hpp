///
/// \file InvalidConversion.hpp
/// \author Christian von Arnim
/// \date 01.08.2018
///

#pragma once

#include "SeRoNetSDKException.hpp"

namespace SeRoNet {
namespace Exceptions {
/// Exception, which is thrown, when an invalid conversion is requested
class InvalidConversion : public SeRoNetSDKException {
 public:
  using SeRoNetSDKException::SeRoNetSDKException;
};
}
}
