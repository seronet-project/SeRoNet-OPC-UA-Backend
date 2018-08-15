//
// Created by ac120371 on 14.08.2018.
//
#include "ParameterResponseIdlDescription.hpp"
#include "../SelfDescriptionArray.hpp"
#include "../SelfDescriptionPrimitives.hpp"
#include "../ComplexType.hpp"

namespace SeRoNet {
namespace CommunicationObjects {
namespace Description {

template<>
CommunicationObjects::Description::IDescription::shp_t SelfDescription(
    CommunicationObjects::DefaultObjects::CommParameterIDL::CommParameterResponse *obj,
    std::string name)
    {

  CommunicationObjects::Description::ComplexType::shp_t ret(
      new ComplexType(name)
  );

  // Add Members
  (*ret).add(
      SelfDescription(&obj->response, "response")
  );
  return ret;
}

}
}
}
