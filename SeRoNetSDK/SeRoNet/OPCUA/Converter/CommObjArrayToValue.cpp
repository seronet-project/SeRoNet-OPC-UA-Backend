///
/// \file CommObjArrayToValue.cpp
/// \author Christian von Arnim
/// \date 03.08.2018
///

#include "CommObjArrayToValue.hpp"
#include "SeRoNet/CommunicationObjects/Description/IVisitorDescription.hpp"
#include "SeRoNet/CommunicationObjects/Description/ElementPrimitive.hpp"
#include "SeRoNet/Exceptions/NotImplementedException.hpp"
namespace SeRoNet {
namespace OPCUA {
namespace Server {

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

  void visit(CommunicationObjects::Description::ElementPrimitive<int32_t> *el) override {
    setArrayType(UA_TYPES_INT32);
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

  void setNextIndex(std::size_t index)
  {
    m_index = index;
  }

 protected:
  UA_Variant *m_pVariant;
  std::size_t m_index = 0;

 public:
  void visit(CommunicationObjects::Description::ComplexType *complexObject) override {
    ///\todo use CommToVariantArray
    throw Exceptions::NotImplementedException("Set ComplexObj as Array Element");
  }

  void visit(CommunicationObjects::Description::ElementArray *elementArray) override {
    ///\todo use CommObjArrayToValue
    CommObjArrayToValue co2Arr(elementArray);
    UA_Variant *pVariant = reinterpret_cast<UA_Variant*> (m_pVariant->data);
    auto src = co2Arr.Value();
    UA_Variant_copy(&src, &pVariant[m_index]);
  }

  void visit(CommunicationObjects::Description::ElementPrimitive<bool> *el) override {
    bool *pBool = reinterpret_cast<bool*> (m_pVariant->data);
    pBool[m_index] = el->get();
  }

  void visit(CommunicationObjects::Description::ElementPrimitive<int32_t> *el) override {
    std::int32_t *pInt32 = reinterpret_cast<std::int32_t *> (m_pVariant->data);
    pInt32[m_index] = el->get();
  }

  void visit(CommunicationObjects::Description::ElementPrimitive<float> *el) override {
    float *pFloat = reinterpret_cast<float *> (m_pVariant->data);
    pFloat[m_index] = el->get();
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
    arr->accept(&initVisitor);
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

}
}
}
