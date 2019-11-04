/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#include <open62541/types_generated_handling.h>
#include "CommObjArrayToValue.hpp"
#include "../../CommunicationObjects/Description/IVisitorDescription.hpp"
#include "../../CommunicationObjects/Description/ComplexType.hpp"
#include "../../CommunicationObjects/Description/ElementPrimitive.hpp"
#include "../../Exceptions/NotImplementedException.hpp"
#include "CommObjectToUaVariantArray.hpp"
namespace SeRoNet {
namespace OPCUA {
namespace Converter {

class InitializeArrayVisitor : public CommunicationObjects::Description::IVisitorDescription {
 public:
  InitializeArrayVisitor(UA_Variant *pVariant, std::size_t size)
      : m_pVariant(pVariant), m_size(size) {

  }

 protected:
  UA_Variant *m_pVariant;
  std::size_t m_size;

  void setArrayType(std::size_t ua_type_index) {
    UA_Variant_setArray(
        m_pVariant,
        UA_Array_new(m_size, &UA_TYPES[ua_type_index]),
        m_size,
        &UA_TYPES[ua_type_index]
    );
    m_pVariant->arrayDimensions = (UA_UInt32 *) UA_Array_new(1, &UA_TYPES[UA_TYPES_UINT32]);
    m_pVariant->arrayDimensionsSize = 1;
    m_pVariant->arrayDimensions[0] = m_size;
  }

  void visit(CommunicationObjects::Description::ComplexType *complexObject) override {
    setArrayType(UA_TYPES_VARIANT);
  }
  void visit(CommunicationObjects::Description::ElementArray *elementArray) override {
    setArrayType(UA_TYPES_VARIANT); // Could be unequal size, so variant needed
  }

  void visit(CommunicationObjects::Description::ElementPrimitive<bool> *el) override {
    setArrayType(UA_TYPES_BOOLEAN);
  }

  void visit(CommunicationObjects::Description::ElementPrimitive<int8_t> *el) override {
    setArrayType(UA_TYPES_BYTE);
  }

  void visit(CommunicationObjects::Description::ElementPrimitive<int16_t> *el) override {
    setArrayType(UA_TYPES_INT16);
  }

  void visit(CommunicationObjects::Description::ElementPrimitive<int32_t> *el) override {
    setArrayType(UA_TYPES_INT32);
  }

  void visit(CommunicationObjects::Description::ElementPrimitive<int64_t> *el) override {
    setArrayType(UA_TYPES_INT64);
  }

  void visit(CommunicationObjects::Description::ElementPrimitive<uint8_t> *el) override {
    setArrayType(UA_TYPES_BYTE);
  }

  void visit(CommunicationObjects::Description::ElementPrimitive<uint16_t> *el) override {
    setArrayType(UA_TYPES_UINT16);
  }
  void visit(CommunicationObjects::Description::ElementPrimitive<uint32_t> *el) override {
    setArrayType(UA_TYPES_UINT32);
  }
  void visit(CommunicationObjects::Description::ElementPrimitive<uint64_t> *el) override {
    setArrayType(UA_TYPES_UINT64);
  }

  void visit(CommunicationObjects::Description::ElementPrimitive<float> *el) override {
    setArrayType(UA_TYPES_FLOAT);
  }

  void visit(CommunicationObjects::Description::ElementPrimitive<double> *el) override {
    setArrayType(UA_TYPES_DOUBLE);
  }

  void visit(CommunicationObjects::Description::ElementPrimitive<std::string> *el) override {
    setArrayType(UA_TYPES_STRING);
  }
};

class SetArrayElementVisitor : public CommunicationObjects::Description::IVisitorDescription {
 public:
  SetArrayElementVisitor(UA_Variant *pVariant)
      : m_pVariant(pVariant) {

  }

  void setNextIndex(std::size_t index) {
    m_index = index;
  }

 protected:
  UA_Variant *m_pVariant;
  std::size_t m_index = 0;

 public:
  void visit(CommunicationObjects::Description::ComplexType *complexObject) override {
    UA_Variant *pVariant = &(reinterpret_cast<UA_Variant *> (m_pVariant->data)[m_index]);
    auto variantArray = SeRoNet::OPCUA::Converter::CommObjectToUaVariantArray(complexObject).getValue();
    UA_Variant_setArrayCopy(pVariant, variantArray.Variants, variantArray.VariantsSize, &UA_TYPES[UA_TYPES_VARIANT]);
    pVariant->arrayDimensions = (UA_UInt32 *)UA_Array_new(1, &UA_TYPES[UA_TYPES_UINT32]);
    pVariant->arrayDimensions[0] = variantArray.VariantsSize;
    pVariant->arrayDimensionsSize = 1;
  }

  void visit(CommunicationObjects::Description::ElementArray *elementArray) override {
    CommObjArrayToValue co2Arr(elementArray);
    UA_Variant *pVariant = reinterpret_cast<UA_Variant *> (m_pVariant->data);
    auto src = co2Arr.Value();
    UA_Variant_copy(&src, &pVariant[m_index]);
  }

  void visit(CommunicationObjects::Description::ElementPrimitive<bool> *el) override {
    bool *pBool = reinterpret_cast<bool *> (m_pVariant->data);
    pBool[m_index] = el->get();
  }

  void visit(CommunicationObjects::Description::ElementPrimitive<int8_t> *el) override {
    std::int8_t *tmp = reinterpret_cast<std::int8_t *> (m_pVariant->data);
    tmp[m_index] = el->get();
  }

  void visit(CommunicationObjects::Description::ElementPrimitive<int16_t> *el) override {
    std::int16_t *tmp = reinterpret_cast<std::int16_t *> (m_pVariant->data);
    tmp[m_index] = el->get();
  }

  void visit(CommunicationObjects::Description::ElementPrimitive<int32_t> *el) override {
    std::int32_t *pInt32 = reinterpret_cast<std::int32_t *> (m_pVariant->data);
    pInt32[m_index] = el->get();
  }

  void visit(CommunicationObjects::Description::ElementPrimitive<int64_t> *el) override {
    std::int64_t *tmp = reinterpret_cast<std::int64_t *> (m_pVariant->data);
    tmp[m_index] = el->get();
  }
  void visit(CommunicationObjects::Description::ElementPrimitive<uint8_t > *el) override {
    std::uint8_t *tmp = reinterpret_cast<std::uint8_t *> (m_pVariant->data);
    tmp[m_index] = el->get();
  }
  void visit(CommunicationObjects::Description::ElementPrimitive<uint16_t> *el) override {
    std::uint16_t *tmp = reinterpret_cast<std::uint16_t *> (m_pVariant->data);
    tmp[m_index] = el->get();
  }
  void visit(CommunicationObjects::Description::ElementPrimitive<uint32_t> *el) override {
    std::uint32_t *tmp = reinterpret_cast<std::uint32_t *> (m_pVariant->data);
    tmp[m_index] = el->get();
  }
  void visit(CommunicationObjects::Description::ElementPrimitive<uint64_t> *el) override {
    std::uint64_t *tmp = reinterpret_cast<std::uint64_t *> (m_pVariant->data);
    tmp[m_index] = el->get();
  }

  void visit(CommunicationObjects::Description::ElementPrimitive<float> *el) override {
    float *tmp = reinterpret_cast<float *> (m_pVariant->data);
    tmp[m_index] = el->get();
  }

  void visit(CommunicationObjects::Description::ElementPrimitive<double> *el) override {
    double *pDouble = reinterpret_cast<double *> (m_pVariant->data);
    pDouble[m_index] = el->get();
  }

  void visit(CommunicationObjects::Description::ElementPrimitive<std::string> *el) override {
    UA_String *pString = reinterpret_cast<UA_String *> (m_pVariant->data);
    pString[m_index] = UA_STRING_ALLOC(el->get().c_str());
  }
};

CommObjArrayToValue::CommObjArrayToValue(CommunicationObjects::Description::ElementArray *arr) {
  UA_Variant_init(&m_variant);
  {
    InitializeArrayVisitor initVisitor(&m_variant, arr->size());
    // Visit with an element once to determine the type
    arr->getNewElement()->getDescription()->accept(&initVisitor);
  }

  {
    SetArrayElementVisitor setElVisitor(&m_variant);
    for (std::size_t i = 0; i < arr->size(); ++i) {
      setElVisitor.setNextIndex(i);
      auto el = arr->getElement(i);
      el->getDescription()->accept(&setElVisitor);
    }
  }

}

CommObjArrayToValue::~CommObjArrayToValue() {
  UA_Variant_deleteMembers(&m_variant);
}

UA_Variant CommObjArrayToValue::Value() {
  return m_variant;
}

}  // namespace Converter
}  // namespace OPCUA
}  // namespace SeRoNet
