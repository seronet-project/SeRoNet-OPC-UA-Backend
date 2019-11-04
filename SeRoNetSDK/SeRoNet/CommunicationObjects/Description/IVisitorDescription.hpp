/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#pragma once

#include <cstdint>
#include <string>

namespace SeRoNet {
namespace CommunicationObjects {
namespace Description {
template<typename T>
class ElementPrimitive;

class ComplexType;
class ElementArray;

class IVisitorDescription {
 public:
  virtual void visit(ComplexType *complexObject) = 0;
  virtual void visit(ElementArray *elementArray) = 0;
  virtual void visit(ElementPrimitive<bool> *el) = 0;

  virtual void visit(ElementPrimitive<std::int8_t> *el) = 0;
  virtual void visit(ElementPrimitive<std::int16_t> *el) = 0;
  virtual void visit(ElementPrimitive<std::int32_t> *el) = 0;
  virtual void visit(ElementPrimitive<std::int64_t> *el) = 0;

  virtual void visit(ElementPrimitive<std::uint8_t> *el) = 0;
  virtual void visit(ElementPrimitive<std::uint16_t> *el) = 0;
  virtual void visit(ElementPrimitive<std::uint32_t> *el) = 0;
  virtual void visit(ElementPrimitive<std::uint64_t> *el) = 0;



  virtual void visit(ElementPrimitive<float> *el) = 0;
  virtual void visit(ElementPrimitive<double> *el) = 0;

  virtual void visit(ElementPrimitive<std::string> *el) = 0;
  /// \todo declare all types

};
}
}
}
