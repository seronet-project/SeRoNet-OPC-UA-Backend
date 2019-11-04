/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#pragma once

#include "IDescription.hpp"
#include <functional>

namespace SeRoNet {
namespace CommunicationObjects {
namespace Description {

/// Base Class for all Primitives, allow dynamic_cast checking for primitve types
class ElementPrimitiveBase : public IDescription {
  using IDescription::IDescription;
};

template<typename PRIMITIVE_TYPE>
class ElementPrimitive : public ElementPrimitiveBase {
 public:
  ElementPrimitive(std::string name, std::function<PRIMITIVE_TYPE()> getter,
                   std::function<void(PRIMITIVE_TYPE)> setter) :
      ElementPrimitiveBase(std::move(name)), m_getter(std::move(getter)), m_setter(std::move(setter)) {}

  PRIMITIVE_TYPE get() const { return m_getter(); };
  void set(PRIMITIVE_TYPE newValue) const { m_setter(newValue); };

  void accept(IVisitorDescription *visitor) override {
    visitor->visit(this);
  }

 private:
  const std::function<PRIMITIVE_TYPE()> m_getter;
  const std::function<void(PRIMITIVE_TYPE)> m_setter;
};
}
}
}
