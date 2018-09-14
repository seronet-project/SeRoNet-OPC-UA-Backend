//
// Created by ac120371 on 14.08.2018.
//

#include "ParameterRequestDescription.hpp"
#include "ParameterRequestIdlDescription.hpp"

namespace SeRoNet {
namespace CommunicationObjects {
namespace Description {

template<>
IDescription::shp_t SelfDescription<
    SeRoNet::DefaultCommObjects::CommParameterRequest>(
    SeRoNet::DefaultCommObjects::CommParameterRequest *obj,
    std::string name) {

  ComplexType::shp_t ret(
      new ComplexType(name)
  );

  // Add Member

  (*ret).add(
      SelfDescription(&obj->data, "data")
  );

  return ret;
}

}
}
}
