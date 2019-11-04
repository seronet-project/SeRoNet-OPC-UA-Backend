/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#ifndef SERONETSDK_ENUMPARAMRESPONSETYPEBASE_H
#define SERONETSDK_ENUMPARAMRESPONSETYPEBASE_H

namespace SeRoNet {
namespace DefaultCommObjects {

enum ENUM_INTERNAL_ParamResponseType {
  LITERAL_BUSY,
  LITERAL_INVALID,
  LITERAL_OK
};
typedef int ENUM_ParamResponseType;

} // end namespace DefaultCommObjects
} // end namespace SeRoNet

#endif //SERONETSDK_ENUMPARAMRESPONSETYPEBASE_H
