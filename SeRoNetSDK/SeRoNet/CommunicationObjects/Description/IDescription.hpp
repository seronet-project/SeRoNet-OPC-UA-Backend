/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#pragma once

#include "IVisitableDescription.hpp"
#include <memory>

namespace SeRoNet {
namespace CommunicationObjects {
namespace Description {
class IDescription : public IVisitableDescription {
 public:
  typedef std::shared_ptr<IDescription> shp_t;
  explicit IDescription(std::string name) : m_name(std::move(name)) {}

  const std::string &getName() const {
    return m_name;
  }

 private:
  const std::string m_name;
};

}
}
}
