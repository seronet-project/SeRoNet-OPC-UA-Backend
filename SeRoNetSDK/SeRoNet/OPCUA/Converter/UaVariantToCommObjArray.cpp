/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/


#include "UaVariantToCommObjArray.hpp"
#include "../../Exceptions/NotImplementedException.hpp"
#include "../../Exceptions/InvalidConversion.hpp"
#include "../../CommunicationObjects/Description/IVisitorDescription.hpp"
#include "../../CommunicationObjects/Description/ComplexType.hpp"
#include "UaVariantArrayToCommObject.hpp"
#include "../../CommunicationObjects/Description/ElementPrimitive.hpp"

namespace SeRoNet {
namespace OPCUA {
namespace Converter {

namespace internal {

class ToCommObjArrayVisitor : public CommunicationObjects::Description::IVisitorDescription {
 public:
  ToCommObjArrayVisitor(
      CommunicationObjects::Description::ElementArray *arr, open62541Cpp::UA_Variant &variant)
      : m_variant(variant) {

    for (m_index = 0; m_index < variant.Variant->arrayLength; ++m_index) {
      auto newEl = arr->getNewElement();
      auto elDescription = newEl->getDescription();
      elDescription->accept(this);
      arr->push_back(newEl);
    }

    arr->setVector();
  }

  void visit(CommunicationObjects::Description::ComplexType *complexObject) override {
    if (m_variant.Variant->type->typeIndex != UA_TYPES[UA_TYPES_VARIANT].typeIndex) {
      throw SeRoNet::Exceptions::InvalidConversion("Expected Variant");
    }
    auto pData = m_variant.getDataAs<UA_Variant>();
    if (pData->arrayDimensionsSize != 1 || !pData->arrayDimensions) {
      throw SeRoNet::Exceptions::InvalidConversion("Expected single dimension array of Variants");
    }
    open62541Cpp::UA_ArrayOfVariant
        ua_arrayOfVariant(reinterpret_cast<UA_Variant *> (pData[m_index].data), pData[m_index].arrayLength);
    Converter::UaVariantArrayToCommObject uaVariantArrayToCommObject(ua_arrayOfVariant, complexObject);
  }

  void visit(CommunicationObjects::Description::ElementArray *elementArray) override {
    if (m_variant.Variant->type->typeIndex != UA_TYPES[UA_TYPES_VARIANT].typeIndex) {
      throw SeRoNet::Exceptions::InvalidConversion("Expected Variant");
    }
    auto pData = m_variant.getDataAs<UA_Variant>();
    open62541Cpp::UA_Variant ua_variant(&pData[m_index], false);
    VariantToCommObjArray valToCommObj(elementArray, ua_variant);
  }

  void visit(CommunicationObjects::Description::ElementPrimitive<bool> *el) override {
    if (m_variant.Variant->type->typeIndex != UA_TYPES[UA_TYPES_BOOLEAN].typeIndex) {
      throw SeRoNet::Exceptions::InvalidConversion("Expected Bool");
    }
    auto pData = m_variant.getDataAs<UA_Boolean>();
    el->set(pData[m_index] == UA_TRUE);
  }

  void visit(CommunicationObjects::Description::ElementPrimitive<int8_t> *el) override {
    if (m_variant.Variant->type->typeIndex != UA_TYPES[UA_TYPES_BYTE].typeIndex) {
      throw SeRoNet::Exceptions::InvalidConversion("Expected int8");
    }
    auto pData = m_variant.getDataAs<std::int8_t>();
    el->set(pData[m_index]);
  }

  void visit(CommunicationObjects::Description::ElementPrimitive<int16_t> *el) override {
    if (m_variant.Variant->type->typeIndex != UA_TYPES[UA_TYPES_INT16].typeIndex) {
      throw SeRoNet::Exceptions::InvalidConversion("Expected int16");
    }
    auto pData = m_variant.getDataAs<std::int16_t>();
    el->set(pData[m_index]);
  }

  void visit(CommunicationObjects::Description::ElementPrimitive<int32_t> *el) override {
    if (m_variant.Variant->type->typeIndex != UA_TYPES[UA_TYPES_INT32].typeIndex) {
      throw SeRoNet::Exceptions::InvalidConversion("Expected int32");
    }
    auto pData = m_variant.getDataAs<std::int32_t>();
    el->set(pData[m_index]);
  }

  void visit(CommunicationObjects::Description::ElementPrimitive<int64_t> *el) override {
    if (m_variant.Variant->type->typeIndex != UA_TYPES[UA_TYPES_INT64].typeIndex) {
      throw SeRoNet::Exceptions::InvalidConversion("Expected int32");
    }
    auto pData = m_variant.getDataAs<std::int32_t>();
    el->set(pData[m_index]);
  }

  void visit(CommunicationObjects::Description::ElementPrimitive<uint8_t> *el) override {
    if (m_variant.Variant->type->typeIndex != UA_TYPES[UA_TYPES_BYTE].typeIndex) {
      throw SeRoNet::Exceptions::InvalidConversion("Expected int8");
    }
    auto pData = m_variant.getDataAs<std::uint8_t>();
    el->set(pData[m_index]);
  }

  void visit(CommunicationObjects::Description::ElementPrimitive<uint16_t> *el) override {
    if (m_variant.Variant->type->typeIndex != UA_TYPES[UA_TYPES_UINT16].typeIndex) {
      throw SeRoNet::Exceptions::InvalidConversion("Expected uint16_t");
    }
    auto pData = m_variant.getDataAs<std::uint16_t>();
    el->set(pData[m_index]);
  }

  void visit(CommunicationObjects::Description::ElementPrimitive<uint32_t> *el) override {
    if (m_variant.Variant->type->typeIndex != UA_TYPES[UA_TYPES_UINT32].typeIndex) {
      throw SeRoNet::Exceptions::InvalidConversion("Expected uint32_t");
    }
    auto pData = m_variant.getDataAs<std::uint32_t>();
    el->set(pData[m_index]);
  }

  void visit(CommunicationObjects::Description::ElementPrimitive<uint64_t> *el) override {
    if (m_variant.Variant->type->typeIndex != UA_TYPES[UA_TYPES_UINT64].typeIndex) {
      throw SeRoNet::Exceptions::InvalidConversion("Expected uint64_t");
    }
    auto pData = m_variant.getDataAs<std::uint64_t>();
    el->set(pData[m_index]);
  }

  void visit(CommunicationObjects::Description::ElementPrimitive<float> *el) override {
    if (m_variant.Variant->type->typeIndex != UA_TYPES[UA_TYPES_FLOAT].typeIndex) {
      throw SeRoNet::Exceptions::InvalidConversion("Expected float");
    }
    auto pData = m_variant.getDataAs<float>();
    el->set(pData[m_index]);
  }

  void visit(CommunicationObjects::Description::ElementPrimitive<double> *el) override {
    if(m_variant.Variant->type->typeIndex != UA_TYPES[UA_TYPES_DOUBLE].typeIndex)
    {
      throw SeRoNet::Exceptions::InvalidConversion("Expected double");
    }
    auto pData = m_variant.getDataAs<double>();
    el->set(pData[m_index]);
  }

  void visit(CommunicationObjects::Description::ElementPrimitive<std::string> *el) override {
    if(m_variant.Variant->type->typeIndex != UA_TYPES[UA_TYPES_STRING].typeIndex)
    {
      throw SeRoNet::Exceptions::InvalidConversion("Expected int32");
    }
    auto pData = m_variant.getDataAs<UA_String>();

    el->set(
        std::string(
            reinterpret_cast<char*>(pData[m_index].data),
            pData[m_index].length
            )
        );
  }

 protected:
  std::size_t m_index;
  open62541Cpp::UA_Variant &m_variant;
};

}

VariantToCommObjArray::VariantToCommObjArray(
    CommunicationObjects::Description::ElementArray *arr,
    open62541Cpp::UA_Variant &variant) {
  internal::ToCommObjArrayVisitor toCommObjArrayVisitor(arr, variant);
}

} //  namespace Converter
} //  namespace OPCUA
} //  namespace SeRoNet