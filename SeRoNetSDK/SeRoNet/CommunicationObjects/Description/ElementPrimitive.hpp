///
/// \file ElementPrimitive.hpp
/// \author Christian von Arnim
/// \date 15.02.2018
///

#pragma once

#include "IDescription.hpp"
#include <functional>

namespace SeRoNet {
namespace CommunicationObjects {
namespace Description {
template<typename PRIMITIVE_TYPE>
class ElementPrimitive : public IDescription {
 public:
  ElementPrimitive(std::string name, std::function<PRIMITIVE_TYPE()> getter,
                   std::function<void(PRIMITIVE_TYPE)> setter) :
      IDescription(std::move(name)), m_getter(std::move(getter)), m_setter(std::move(setter)) {}

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
