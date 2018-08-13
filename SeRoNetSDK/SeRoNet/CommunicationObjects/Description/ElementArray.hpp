///
/// \file ElementArray.hpp
/// \author Christian von Arnim
/// \date 31.07.2018
///

#pragma once

#include "IDescription.hpp"
#include <functional>

#include "SelfDescription.hpp"
#include <vector>
#include "../../Exceptions/InvalidConversion.hpp"
#include "../../Exceptions/OutOfRange.hpp"
#include <memory>

namespace SeRoNet {
namespace CommunicationObjects {
namespace Description {

/// Interface Version of array Self description, without template argument!
class ElementArray : public IDescription {

 public:
  class ArrayElement_t {
   public:
    typedef std::shared_ptr<ArrayElement_t> shp_t;
    virtual IDescription::shp_t getDescription() = 0;
  };

  ElementArray(std::string name) :
      IDescription(std::move(name)) {}

  void accept(IVisitorDescription *visitor) override {
    visitor->visit(this);
  }

  /// Copy the vector to the internal memory
  virtual void acquireVector() = 0;
  /// Write the internal vector to the source
  virtual void setVector() = 0;
  virtual ArrayElement_t::shp_t getElement(std::size_t index) = 0;
  virtual ArrayElement_t::shp_t getNewElement() = 0;
  virtual void push_back(ArrayElement_t::shp_t) = 0;
  virtual void clear() = 0;
  virtual std::size_t size() = 0;
};

/// Implementation with template argument, and std::vector
template<typename TYPE>
class ArrayOf : public ElementArray {
 public:
  typedef std::function<std::vector<TYPE>()> getter_t;
  typedef std::function<void(std::vector<TYPE>)> setter_t;

  ArrayOf(std::string name, getter_t getter, setter_t setter)
      : ElementArray(name), m_set(setter), m_get(getter) {
    // Read vector to memory
    acquireVector();
  }

  class ArrayElementOf : public ArrayElement_t {
   public:
    ArrayElementOf() = default;
    ArrayElementOf(const TYPE &otherEl)
        : el(otherEl) {}
    IDescription::shp_t getDescription() override {
      return SelfDescription(&el);
    }

    TYPE &Value() { return el; }

   protected:
    TYPE el;
  };

  ArrayElement_t::shp_t getElement(std::size_t index) override {
    if (index >= m_vec.size()) {
      throw Exceptions::OutOfRange("Index out of range.");
    }
    return std::make_shared<ArrayElementOf>(m_vec[index]);
  }

  ArrayElement_t::shp_t getNewElement() override {
    return std::make_shared<ArrayElementOf>();
  }

  void push_back(ArrayElement_t::shp_t el) override {
    auto castedEl = std::dynamic_pointer_cast<ArrayElementOf>(el);
    if (!castedEl) {
      throw Exceptions::InvalidConversion("ArrayElement has the wrong type!");
    }

    m_vec.push_back(castedEl->Value());
  }

  void clear() override {
    m_vec.clear();
  }

  size_t size() override {
    return m_vec.size();
  }

  void acquireVector() override {
    m_vec = m_get();
  }

  void setVector() override {
    m_set(m_vec);
  }

 protected:
  getter_t m_get;
  setter_t m_set;
  std::vector<TYPE> m_vec;
};

}
}
}
