///
/// \file CommObjectToUaVariantArray.cpp
/// \author Christian von Arnim
/// \date 16.02.2018
///

#include "CommObjectToUaVariantArray.hpp"
#include "../../../CommunicationObjects/Description/IVisitorDescription.hpp"
#include "../../../Exceptions/NotImplementedException.hpp"
#include <list>
#include <Open62541Cpp/UA_String.hpp>
#include "../../../CommunicationObjects/Description/ComplexType.hpp"
#include "../../../CommunicationObjects/Description/ElementPrimitive.hpp"
#include "../../../CommunicationObjects/Description/ElementArray.hpp"

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

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<int32_t> *el) override {
    open62541::UA_Variant newEl;
    auto value = el->get();
    UA_Variant_setScalarCopy(newEl.Variant, &value, &UA_TYPES[UA_TYPES_INT32]);
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
    throw SeRoNet::Exceptions::NotImplementedException(__FUNCTION__);
  }

  std::list<open62541::UA_Variant> Variants;
};

namespace SeRoNet {
namespace OPCUA {
namespace Client {
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

}
}
}
}
