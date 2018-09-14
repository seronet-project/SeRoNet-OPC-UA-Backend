//
// Created by ac120371 on 14.08.2018.
//
#include "ParameterResponseDescription.hpp"
#include "ParameterResponseIdlDescription.hpp"
#include "../../CommunicationObjects/Description/ComplexType.hpp"

namespace SeRoNet {
namespace CommunicationObjects {
namespace Description {

template<>
IDescription::shp_t SelfDescription(
        SeRoNet::DefaultCommObjects::CommParameterResponse *obj,
        std::string name
    ) {

  ComplexType::shp_t ret(
      new ComplexType(name)
  );

//  Add Members
  (*ret).add(
      SelfDescription(&obj->data, "data")
  );

  return ret;
}

}
}
}
