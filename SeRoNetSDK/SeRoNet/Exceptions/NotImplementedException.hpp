///
/// \file NotImplementedException.hpp
/// \author Christian von Arnim
/// \date 16.02.2018
///

#pragma once
#include "SeRoNetSDKException.hpp"

namespace SeRoNet {
namespace Exceptions {

class NotImplementedException : public SeRoNetSDKException {
 public:
  using SeRoNetSDKException::SeRoNetSDKException;
};

}
}

