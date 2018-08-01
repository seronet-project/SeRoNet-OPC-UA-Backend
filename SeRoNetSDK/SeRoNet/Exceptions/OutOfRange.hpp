///
/// \file OutOfRange.hpp
/// \author Christian von Arnim
/// \date 01.08.2018
///

#pragma once

#include "SeRoNetSDKException.hpp"

namespace SeRoNet {
namespace Exceptions {

class OutOfRange : public SeRoNetSDKException {
 public:
  using SeRoNetSDKException::SeRoNetSDKException;
};
}
}
