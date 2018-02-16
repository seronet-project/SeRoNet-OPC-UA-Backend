///
/// \file UaVariantArrayToCoordinateObject.cpp
/// \author Christian von Arnim
/// \date 16.02.2018
///

#include "UaVariantArrayToCoordinateObject.hpp"

#include "../../../CommunicationObjects/Description/IVisitorDescription.hpp"
#include "../../../Exceptions/NotImplementedException.hpp"
#include <iostream>
#include "../../../CommunicationObjects/Description/ComplexType.hpp"
#include "../../../CommunicationObjects/Description/ElementPrimitive.hpp"

/// Internal Class
class ToCoordinateObjectVisitor : public ::SeRoNet::CommunicationObjects::Description::IVisitorDescription {
 public:
  ToCoordinateObjectVisitor(
      open62541::UA_ArrayOfVariant srcVariantArray
  ):
  m_srcVariants(srcVariantArray) {

  }

  void visit(SeRoNet::CommunicationObjects::Description::ComplexType *complexObject) override {
    for (auto &el: *complexObject) {
      el->accept(this);
    }
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

  void visit(SeRoNet::CommunicationObjects::Description::ElementPrimitive<std::string> *el) override {
    throw ::SeRoNet::Exceptions::NotImplementedException(std::string(__FUNCTION__) + " not implemented");
  }

  open62541::UA_ArrayOfVariant m_srcVariants;
  std::size_t m_nextIndex = 0;
};

namespace SeRoNet {
namespace OPCUA {
namespace Client {
namespace Converter {

UaVariantArrayToCoordinateObject::UaVariantArrayToCoordinateObject(
    open62541::UA_ArrayOfVariant src,
    CommunicationObjects::Description::IVisitableDescription *target) {

  ToCoordinateObjectVisitor visitor(src);
  target->accept(&visitor);

}

}
}
}
}
