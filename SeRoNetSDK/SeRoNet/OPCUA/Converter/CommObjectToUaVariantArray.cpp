/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/


#include "CommObjectToUaVariantArray.hpp"
#include "../../CommunicationObjects/Description/IVisitorDescription.hpp"
#include "../../Exceptions/NotImplementedException.hpp"
#include <list>
#include <Open62541Cpp/UA_String.hpp>
#include "../../CommunicationObjects/Description/ComplexType.hpp"
#include "../../CommunicationObjects/Description/ElementPrimitive.hpp"
#include "../../CommunicationObjects/Description/ElementArray.hpp"
#include "CommObjArrayToValue.hpp"

#include <open62541/types_generated_handling.h>

/// Internal Class
class ToUaVariantArrayVisitor : public ::SeRoNet::CommunicationObjects::Description::IVisitorDescription {
 public:
  void visit(SeRoNet::CommunicationObjects::Description::ComplexType *complexObject) override {
    for (auto &el: *complexObject) {
      el->accept(this);
    }
  }
  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<bool> *el) override {
    open62541::UA_Variant newEl;
    auto value = el->get();
    UA_Variant_setScalarCopy(newEl.Variant, &value, &UA_TYPES[UA_TYPES_BOOLEAN]);
    Variants.push_back(newEl);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<int8_t> *el) override {
    open62541::UA_Variant newEl;
    auto value = el->get();
    UA_Variant_setScalarCopy(newEl.Variant, &value, &UA_TYPES[UA_TYPES_BYTE]);
    Variants.push_back(newEl);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<int16_t> *el) override {
    open62541::UA_Variant newEl;
    auto value = el->get();
    UA_Variant_setScalarCopy(newEl.Variant, &value, &UA_TYPES[UA_TYPES_INT16]);
    Variants.push_back(newEl);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<int32_t> *el) override {
    open62541::UA_Variant newEl;
    auto value = el->get();
    UA_Variant_setScalarCopy(newEl.Variant, &value, &UA_TYPES[UA_TYPES_INT32]);
    Variants.push_back(newEl);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<int64_t> *el) override {
    open62541::UA_Variant newEl;
    auto value = el->get();
    UA_Variant_setScalarCopy(newEl.Variant, &value, &UA_TYPES[UA_TYPES_INT64]);
    Variants.push_back(newEl);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<uint8_t> *el) override {
    open62541::UA_Variant newEl;
    auto value = el->get();
    UA_Variant_setScalarCopy(newEl.Variant, &value, &UA_TYPES[UA_TYPES_BYTE]);
    Variants.push_back(newEl);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<uint16_t> *el) override {
    open62541::UA_Variant newEl;
    auto value = el->get();
    UA_Variant_setScalarCopy(newEl.Variant, &value, &UA_TYPES[UA_TYPES_UINT16]);
    Variants.push_back(newEl);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<uint32_t> *el) override {
    open62541::UA_Variant newEl;
    auto value = el->get();
    UA_Variant_setScalarCopy(newEl.Variant, &value, &UA_TYPES[UA_TYPES_UINT32]);
    Variants.push_back(newEl);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<uint64_t> *el) override {
    open62541::UA_Variant newEl;
    auto value = el->get();
    UA_Variant_setScalarCopy(newEl.Variant, &value, &UA_TYPES[UA_TYPES_UINT64]);
    Variants.push_back(newEl);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<float> *el) override {
    open62541::UA_Variant newEl;
    auto value = el->get();
    UA_Variant_setScalarCopy(newEl.Variant, &value, &UA_TYPES[UA_TYPES_FLOAT]);
    Variants.push_back(newEl);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<double> *el) override {
    open62541::UA_Variant newEl;
    auto value = el->get();
    UA_Variant_setScalarCopy(newEl.Variant, &value, &UA_TYPES[UA_TYPES_DOUBLE]);
    Variants.push_back(newEl);
  }


  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<std::string> *el) override {
    open62541::UA_Variant newEl;
    auto value = el->get();
    UA_Variant_setScalarCopy(newEl.Variant, open62541::UA_String(value).String, &UA_TYPES[UA_TYPES_STRING]);
    Variants.push_back(newEl);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementArray *elementArray) override {
    SeRoNet::OPCUA::Converter::CommObjArrayToValue commObjArrayToValue(elementArray);
    auto variant = commObjArrayToValue.Value();
    open62541::UA_Variant newEl(&variant, false);
    Variants.push_back(newEl);
  }

  std::list<open62541::UA_Variant> Variants;
};

namespace SeRoNet {
namespace OPCUA {
namespace Converter {

CommObjectToUaVariantArray::CommObjectToUaVariantArray(
    CommunicationObjects::Description::IVisitableDescription *description
) {
  ToUaVariantArrayVisitor visitor;
  description->accept(&visitor);
  m_convertedValue.Variants =
      reinterpret_cast<UA_Variant *> (UA_Array_new(visitor.Variants.size(), &UA_TYPES[UA_TYPES_VARIANT]));
  m_convertedValue.VariantsSize = visitor.Variants.size();
  std::size_t index = 0;
  for (auto &variant : visitor.Variants) {
    UA_Variant_copy(variant.Variant, &m_convertedValue.Variants[index]);
    ++index;
  }
}

} //  namespace Converter
} //  namespace OPCUA
} //  namespace SeRoNet