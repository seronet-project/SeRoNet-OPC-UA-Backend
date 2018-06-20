///
/// @file ResultErrorException.hpp
/// @author Christian von Arnim 
/// @date 20.06.2018
///

#pragma once

#include "SeRoNetSDKException.hpp"

namespace SeRoNet {
namespace Exceptions {
/// Exception, which is thrown, when the result is not availiable because of an error
class ResultErrorException : public SeRoNetSDKException {
 public:
  using SeRoNetSDKException::SeRoNetSDKException;
};
}
}

