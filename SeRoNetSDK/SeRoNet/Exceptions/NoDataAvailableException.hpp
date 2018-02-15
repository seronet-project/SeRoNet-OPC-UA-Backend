///
/// @file BlockingDisabledException.hpp
/// @author Christian von Arnim 
/// @date 05.01.2018
///

#pragma once

#include "SeRoNetSDKException.hpp"

namespace SeRoNet {
namespace Exceptions {
/// Exception, which is thrown, when data is requested and no Data is available
class NoDataAvailableException : public SeRoNetSDKException {
 public:
  using SeRoNetSDKException::SeRoNetSDKException;
};
}
}

