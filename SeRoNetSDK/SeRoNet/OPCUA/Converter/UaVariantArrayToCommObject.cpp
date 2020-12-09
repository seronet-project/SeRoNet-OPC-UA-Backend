/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#include "UaVariantArrayToCommObject.hpp"

#include "../../CommunicationObjects/Description/IVisitorDescription.hpp"
#include "../../Exceptions/NotImplementedException.hpp"
#include <iostream>
#include <Open62541Cpp/UA_String.hpp>
#include "../../CommunicationObjects/Description/ComplexType.hpp"
#include "../../CommunicationObjects/Description/ElementPrimitive.hpp"
#include "UaVariantToCommObjArray.hpp"

/// Internal Class
class ToCommObjectVisitor : public ::SeRoNet::CommunicationObjects::Description::IVisitorDescription {
 public:
  ToCommObjectVisitor(
      open62541Cpp::UA_ArrayOfVariant srcVariantArray
  ):
  m_srcVariants(srcVariantArray) {

  }

  void visit(SeRoNet::CommunicationObjects::Description::ComplexType *complexObject) override {
    for (auto &el: *complexObject) {
      el->accept(this);
    }
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<bool> *el) override {
    assert(m_srcVariants.VariantsSize > m_nextIndex); ///\todo throw exception instead
    auto nextData = m_srcVariants[m_nextIndex];
    ++m_nextIndex;
    if (!nextData.is_a(&UA_TYPES[UA_TYPES_BOOLEAN])) {
      ///\todo throw exception
      std::cout << "ERROR: Wrong Type." << std::endl;
      return;
    }

    el->set(*nextData.getDataAs<UA_Boolean>());
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<int8_t> *el) override {
    assert(m_srcVariants.VariantsSize > m_nextIndex); ///\todo throw exception instead
    auto nextData = m_srcVariants[m_nextIndex];
    ++m_nextIndex;
    if (!nextData.is_a(&UA_TYPES[UA_TYPES_BYTE])) {
      ///\todo throw exception
      std::cout << "ERROR: Wrong Type." << std::endl;
      return;
    }

    el->set(*nextData.getDataAs<UA_Byte>());
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<int16_t> *el) override {
    assert(m_srcVariants.VariantsSize > m_nextIndex); ///\todo throw exception instead
    auto nextData = m_srcVariants[m_nextIndex];
    ++m_nextIndex;
    if (!nextData.is_a(&UA_TYPES[UA_TYPES_INT16])) {
      ///\todo throw exception
      std::cout << "ERROR: Wrong Type." << std::endl;
      return;
    }

    el->set(*nextData.getDataAs<UA_Int16>());
  }


  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<int32_t> *el) override {
    assert(m_srcVariants.VariantsSize > m_nextIndex); ///\todo throw exception instead
    auto nextData = m_srcVariants[m_nextIndex];
    ++m_nextIndex;
    if (!nextData.is_a(&UA_TYPES[UA_TYPES_INT32])) {
      ///\todo throw exception
      std::cout << "ERROR: Wrong Type." << std::endl;
      return;
    }

    el->set(*nextData.getDataAs<UA_Int32>());
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<int64_t> *el) override {
    assert(m_srcVariants.VariantsSize > m_nextIndex); ///\todo throw exception instead
    auto nextData = m_srcVariants[m_nextIndex];
    ++m_nextIndex;
    if (!nextData.is_a(&UA_TYPES[UA_TYPES_INT64])) {
      ///\todo throw exception
      std::cout << "ERROR: Wrong Type." << std::endl;
      return;
    }

    el->set(*nextData.getDataAs<UA_Int64>());
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<uint8_t> *el) override {
    assert(m_srcVariants.VariantsSize > m_nextIndex); ///\todo throw exception instead
    auto nextData = m_srcVariants[m_nextIndex];
    ++m_nextIndex;
    if (!nextData.is_a(&UA_TYPES[UA_TYPES_BYTE])) {
      ///\todo throw exception
      std::cout << "ERROR: Wrong Type." << std::endl;
      return;
    }

    el->set(*nextData.getDataAs<UA_Byte>());
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<uint16_t> *el) override {
    assert(m_srcVariants.VariantsSize > m_nextIndex); ///\todo throw exception instead
    auto nextData = m_srcVariants[m_nextIndex];
    ++m_nextIndex;
    if (!nextData.is_a(&UA_TYPES[UA_TYPES_UINT16])) {
      ///\todo throw exception
      std::cout << "ERROR: Wrong Type." << std::endl;
      return;
    }

    el->set(*nextData.getDataAs<UA_UInt16>());
  }
  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<uint32_t> *el) override {
    assert(m_srcVariants.VariantsSize > m_nextIndex); ///\todo throw exception instead
    auto nextData = m_srcVariants[m_nextIndex];
    ++m_nextIndex;
    if (!nextData.is_a(&UA_TYPES[UA_TYPES_UINT32])) {
      ///\todo throw exception
      std::cout << "ERROR: Wrong Type." << std::endl;
      return;
    }

    el->set(*nextData.getDataAs<UA_UInt32>());
  }
  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<uint64_t> *el) override {
    assert(m_srcVariants.VariantsSize > m_nextIndex); ///\todo throw exception instead
    auto nextData = m_srcVariants[m_nextIndex];
    ++m_nextIndex;
    if (!nextData.is_a(&UA_TYPES[UA_TYPES_UINT64])) {
      ///\todo throw exception
      std::cout << "ERROR: Wrong Type." << std::endl;
      return;
    }

    el->set(*nextData.getDataAs<UA_UInt32>());
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<float> *el) override {
    assert(m_srcVariants.VariantsSize > m_nextIndex); ///\todo throw exception instead
    auto nextData = m_srcVariants[m_nextIndex];
    ++m_nextIndex;
    if (!nextData.is_a(&UA_TYPES[UA_TYPES_FLOAT])) {
      ///\todo throw exception
      std::cout << "ERROR: Wrong Type." << std::endl;
      return;
    }

    el->set(*nextData.getDataAs<UA_Float>());
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<double> *el) override {
    assert(m_srcVariants.VariantsSize > m_nextIndex); ///\todo throw exception instead
    auto nextData = m_srcVariants[m_nextIndex];
    ++m_nextIndex;
    if (!nextData.is_a(&UA_TYPES[UA_TYPES_DOUBLE])) {
      ///\todo throw exception
      std::cout << "ERROR: Wrong Type." << std::endl;
      return;
    }

    el->set(*nextData.getDataAs<UA_Double>());
  }


  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<std::string> *el) override {
    assert(m_srcVariants.VariantsSize > m_nextIndex); ///\todo throw exception instead
    auto nextData = m_srcVariants[m_nextIndex];
    ++m_nextIndex;
    if (!nextData.is_a(&UA_TYPES[UA_TYPES_STRING])) {
      ///\todo throw exception
      std::cout << "ERROR: Wrong Type." << std::endl;
      return;
    }
    open62541Cpp::UA_String tmp = open62541Cpp::UA_String(nextData.getDataAs<UA_String>());

    el->set(static_cast<std::string>(tmp));
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementArray *elementArray) override {
    assert(m_srcVariants.VariantsSize > m_nextIndex); ///\todo throw exception instead
    auto nextData = m_srcVariants[m_nextIndex];
    SeRoNet::OPCUA::Converter::VariantToCommObjArray vartiTocommObjArray(elementArray, nextData);
    ++m_nextIndex;
  }

  open62541Cpp::UA_ArrayOfVariant m_srcVariants;
  std::size_t m_nextIndex = 0;
};

namespace SeRoNet {
namespace OPCUA {
namespace Converter {

UaVariantArrayToCommObject::UaVariantArrayToCommObject(
    open62541Cpp::UA_ArrayOfVariant src,
    CommunicationObjects::Description::IVisitableDescription *target) {

  ToCommObjectVisitor visitor(src);
  target->accept(&visitor);

}

} //  namespace Converter
} //  namespace OPCUA
} //  namespace SeRoNet