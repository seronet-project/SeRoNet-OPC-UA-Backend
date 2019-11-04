/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/


#ifndef SERONETSDK_BLOCKINGDISABLEDEXCEPTION_HPP
#define SERONETSDK_BLOCKINGDISABLEDEXCEPTION_HPP

#include "SeRoNetSDKException.hpp"

namespace SeRoNet {
namespace Exceptions {
/// Exception, which is thrown, when an (blocking) operation can not be completed, because blocking is disabled
class BlockingDisabledException : public SeRoNetSDKException {
 public:
  using SeRoNetSDKException::SeRoNetSDKException;
};
}
}

#endif //SERONETSDK_BLOCKINGDISABLEDEXCEPTION_HPP
