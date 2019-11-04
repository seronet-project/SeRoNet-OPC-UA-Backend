/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/


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

