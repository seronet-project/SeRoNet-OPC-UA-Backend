/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#include <list>
#include <Open62541Cpp/UA_Argument.hpp>
#include <open62541/types_generated_handling.h>

#include "CommObjectToUaArgument.hpp"
#include "../../Exceptions/NotImplementedException.hpp"
#include "../../CommunicationObjects/Description/ComplexType.hpp"
#include "../../CommunicationObjects/Description/ElementPrimitive.hpp"
#include "../../CommunicationObjects/Description/ElementArray.hpp"
#include "CommObjArrayToTypeIndex.hpp"

/// Internal Class
class ToUaArgumentArrayVisitor :
    public ::SeRoNet::CommunicationObjects::Description::IVisitorDescription {
 public:
  void visit(SeRoNet::CommunicationObjects::Description::ComplexType *complexObject) override {
    for (auto &el: *complexObject) {
      el->accept(this);
    }
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<bool> *el) override {
    open62541::UA_Argument newEl;
    newEl.argument->name = UA_STRING_ALLOC(el->getName().c_str());
    newEl.argument->dataType = UA_TYPES[UA_TYPES_BOOLEAN].typeId;
    newEl.argument->valueRank = -1;
    arguments.push_back(newEl);
  }
  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<float> *el) override {
    open62541::UA_Argument newEl;
    newEl.argument->name = UA_STRING_ALLOC(el->getName().c_str());
    newEl.argument->dataType = UA_TYPES[UA_TYPES_FLOAT].typeId;
    newEl.argument->valueRank = -1;
    arguments.push_back(newEl);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<double> *el) override {
    open62541::UA_Argument newEl;
    newEl.argument->name = UA_STRING_ALLOC(el->getName().c_str());
    newEl.argument->dataType = UA_TYPES[UA_TYPES_DOUBLE].typeId;
    newEl.argument->valueRank = -1;
    arguments.push_back(newEl);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<int8_t> *el) override {
    open62541::UA_Argument newEl;
    newEl.argument->name = UA_STRING_ALLOC(el->getName().c_str());
    newEl.argument->dataType = UA_TYPES[UA_TYPES_BYTE].typeId;
    newEl.argument->valueRank = -1;
    arguments.push_back(newEl);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<int16_t> *el) override {
    open62541::UA_Argument newEl;
    newEl.argument->name = UA_STRING_ALLOC(el->getName().c_str());
    newEl.argument->dataType = UA_TYPES[UA_TYPES_INT16].typeId;
    newEl.argument->valueRank = -1;
    arguments.push_back(newEl);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<int32_t> *el) override {
    open62541::UA_Argument newEl;
    newEl.argument->name = UA_STRING_ALLOC(el->getName().c_str());
    newEl.argument->dataType = UA_TYPES[UA_TYPES_INT32].typeId;
    newEl.argument->valueRank = -1;
    arguments.push_back(newEl);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<int64_t> *el) override {
    open62541::UA_Argument newEl;
    newEl.argument->name = UA_STRING_ALLOC(el->getName().c_str());
    newEl.argument->dataType = UA_TYPES[UA_TYPES_INT64].typeId;
    newEl.argument->valueRank = -1;
    arguments.push_back(newEl);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<uint8_t> *el) override {
    open62541::UA_Argument newEl;
    newEl.argument->name = UA_STRING_ALLOC(el->getName().c_str());
    newEl.argument->dataType = UA_TYPES[UA_TYPES_BYTE].typeId;
    newEl.argument->valueRank = -1;
    arguments.push_back(newEl);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<uint16_t> *el) override {
    open62541::UA_Argument newEl;
    newEl.argument->name = UA_STRING_ALLOC(el->getName().c_str());
    newEl.argument->dataType = UA_TYPES[UA_TYPES_UINT16].typeId;
    newEl.argument->valueRank = -1;
    arguments.push_back(newEl);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<uint32_t> *el) override {
    open62541::UA_Argument newEl;
    newEl.argument->name = UA_STRING_ALLOC(el->getName().c_str());
    newEl.argument->dataType = UA_TYPES[UA_TYPES_UINT32].typeId;
    newEl.argument->valueRank = -1;
    arguments.push_back(newEl);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<uint64_t> *el) override {
    open62541::UA_Argument newEl;
    newEl.argument->name = UA_STRING_ALLOC(el->getName().c_str());
    newEl.argument->dataType = UA_TYPES[UA_TYPES_UINT64].typeId;
    newEl.argument->valueRank = -1;
    arguments.push_back(newEl);
  }
  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<std::string> *el) override {
    open62541::UA_Argument newEl;
    newEl.argument->name = UA_STRING_ALLOC(el->getName().c_str());
    newEl.argument->dataType = UA_TYPES[UA_TYPES_STRING].typeId;
    newEl.argument->valueRank = -1;
    arguments.push_back(newEl);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementArray *elementArray) override {
    auto tmpEl = elementArray->getNewElement();
    SeRoNet::OPCUA::Converter::CommObjArrayToTypeIndex commObjToTypeIndex(tmpEl->getDescription().get());
    open62541::UA_Argument newEl;
    newEl.argument->name = UA_STRING_ALLOC(elementArray->getName().c_str());
    newEl.argument->dataType = UA_TYPES[commObjToTypeIndex.Id()].typeId;
    newEl.argument->valueRank = 1;
    arguments.push_back(newEl);
  }

  std::list<open62541::UA_Argument> arguments;
};

namespace SeRoNet {
namespace OPCUA {
namespace Converter {

CommObjectToUaArgumentArray::CommObjectToUaArgumentArray(
    CommunicationObjects::Description::IVisitableDescription *description
) {
  ToUaArgumentArrayVisitor visitor;
  description->accept(&visitor);

  m_convertedValue.arguments = reinterpret_cast<UA_Argument *>( UA_Array_new(visitor.arguments.size(),
                                                                             &UA_TYPES[UA_TYPES_ARGUMENT]));
  m_convertedValue.arraySize = visitor.arguments.size();
  std::size_t index = 0;
  for (auto &argument : visitor.arguments) {
    UA_Argument_copy(argument.argument, &m_convertedValue.arguments[index]);
    ++index;
  }
}

} //  namespace Converter
} //  namespace OPCUA
} //  namespace SeRoNet