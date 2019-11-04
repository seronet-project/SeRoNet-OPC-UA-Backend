/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#pragma once

#include "../../CommunicationObjects/Description/IDescription.hpp"

namespace SeRoNet {
namespace OPCUA {
namespace Converter {
class CommObjArrayToTypeIndex {
 public:
  CommObjArrayToTypeIndex(CommunicationObjects::Description::IDescription *pDescription);
  std::size_t Id();
 protected:
  std::size_t m_id;
};

}  // namespace Converter
}  // namespace OPCUA
}  // namespace SeRoNet
