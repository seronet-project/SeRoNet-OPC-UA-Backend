/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#include "CommObjArrayToTypeIndex.hpp"

#include "../../CommunicationObjects/Description/IVisitorDescription.hpp"
#include <open62541/types_generated.h>

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

  void visit(CommunicationObjects::Description::ElementPrimitive<int8_t> *el) override {
    m_id = UA_TYPES_BYTE;

  }

  void visit(CommunicationObjects::Description::ElementPrimitive<int16_t> *el) override {
    m_id = UA_TYPES_INT16;

  }

  void visit(CommunicationObjects::Description::ElementPrimitive<int32_t> *el) override {
    m_id = UA_TYPES_INT32;
  }

  void visit(CommunicationObjects::Description::ElementPrimitive<int64_t> *el) override {
    m_id = UA_TYPES_INT64;

  }
  void visit(CommunicationObjects::Description::ElementPrimitive<uint8_t> *el) override {
    m_id = UA_TYPES_BYTE;

  }
  void visit(CommunicationObjects::Description::ElementPrimitive<uint16_t> *el) override {
    m_id = UA_TYPES_UINT16;

  }
  void visit(CommunicationObjects::Description::ElementPrimitive<uint32_t> *el) override {
    m_id = UA_TYPES_UINT32;

  }
  void visit(CommunicationObjects::Description::ElementPrimitive<uint64_t> *el) override {
    m_id = UA_TYPES_UINT64;

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
