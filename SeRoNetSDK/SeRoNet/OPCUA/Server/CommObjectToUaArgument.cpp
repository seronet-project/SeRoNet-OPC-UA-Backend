///
/// \file QueryServerModellGenerator.cpp
/// \author Sebastian Friedl
/// \date 16.02.2018
///
#include <list>
#include <Open62541Cpp/UA_Argument.hpp>

#include "CommObjectToUaArgument.hpp"
#include "../../Exceptions/NotImplementedException.hpp"
#include "../../CommunicationObjects/Description/ComplexType.hpp"
#include "../../CommunicationObjects/Description/ElementPrimitive.hpp"

/// Internal Class
class ToUaArgumentArrayVisitor :
    public ::SeRoNet::CommunicationObjects::Description::IVisitorDescription {
 public:
  void visit(SeRoNet::CommunicationObjects::Description::ComplexType *complexObject) override {
    for (auto &el: *complexObject) {
      el->accept(this);
    }
  }
  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<int32_t> *el) override {
    open62541::UA_Argument newEl;
    newEl.argument->name = UA_STRING_ALLOC(el->getName().c_str());
    newEl.argument->dataType = UA_TYPES[UA_TYPES_INT32].typeId;
    newEl.argument->valueRank = -1;
    arguments.push_back(newEl);
  }

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<std::string> *el) override {
    throw ::SeRoNet::Exceptions::NotImplementedException(std::string(__FUNCTION__) + " not implemented");
  }

  std::list<open62541::UA_Argument> arguments;
};

namespace SeRoNet {
namespace OPCUA {
namespace Server {

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

}
}
}
