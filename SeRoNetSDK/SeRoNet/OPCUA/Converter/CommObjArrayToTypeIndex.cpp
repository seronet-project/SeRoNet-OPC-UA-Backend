///
/// \file CommObjArrayToTypeIndex.cpp
/// \author Christian von Arnim
/// \date 03.08.2018
///

#include "CommObjArrayToTypeIndex.hpp"

#include "../../CommunicationObjects/Description/IVisitorDescription.hpp"
#include <open62541/open62541.h>

namespace SeRoNet {
namespace OPCUA {
namespace Converter {

class ToIndexVisitor : public CommunicationObjects::Description::IVisitorDescription {
 public:
  void visit(CommunicationObjects::Description::ComplexType *complexObject) override {
    m_id = UA_TYPES_VARIANT;
  }

  void visit(CommunicationObjects::Description::ElementArray *elementArray) override {
    m_id = UA_TYPES_VARIANT;
  }

  void visit(CommunicationObjects::Description::ElementPrimitive<bool> *el) override {
    m_id = UA_TYPES_BOOLEAN;
  }

  void visit(CommunicationObjects::Description::ElementPrimitive<int32_t> *el) override {
    m_id = UA_TYPES_INT32;
  }

  void visit(CommunicationObjects::Description::ElementPrimitive<float> *el) override {
    m_id = UA_TYPES_FLOAT;
  }

  void visit(CommunicationObjects::Description::ElementPrimitive<double> *el) override {
    m_id = UA_TYPES_DOUBLE;
  }

  void visit(CommunicationObjects::Description::ElementPrimitive<std::string> *el) override {
    m_id = UA_TYPES_STRING;
  }

  std::size_t Id() { return m_id; }
 protected:
  std::size_t m_id;
};

CommObjArrayToTypeIndex::CommObjArrayToTypeIndex(CommunicationObjects::Description::IDescription *pDescription) {
  ToIndexVisitor tiV;
  pDescription->accept(&tiV);
  m_id = tiV.Id();
}
std::size_t CommObjArrayToTypeIndex::Id() {
  return m_id;
}

}  // namespace Converter
}  // namespace OPCUA
}  // namespace SeRoNet
