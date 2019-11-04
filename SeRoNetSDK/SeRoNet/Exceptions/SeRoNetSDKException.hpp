/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/


#ifndef SERONETSDK_SERONETSDKEXCEPTION_HPP
#define SERONETSDK_SERONETSDKEXCEPTION_HPP

#include <stdexcept>
namespace SeRoNet {
namespace Exceptions {
class SeRoNetSDKException : public std::runtime_error {
 public:
  ///@todo Add more information
  using std::runtime_error::runtime_error;
};
}
}

#endif //SERONETSDK_SERONETSDKEXCEPTION_HPP
