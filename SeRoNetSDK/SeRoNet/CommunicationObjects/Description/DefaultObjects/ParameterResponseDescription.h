//
// Created by ac120371 on 13.08.2018.
//

#ifndef SERONETSDK_PARAMETERRESPONSEDESCRIPTION_H
#define SERONETSDK_PARAMETERRESPONSEDESCRIPTION_H

#include "../ComplexType.hpp"
#include "../../DefaultObjects/ParameterResponse.h"
namespace SeRoNet {
namespace CommunicationObjects {
namespace Description {
// serialization for CommBasePose
template<>
SeRoNet::CommunicationObjects::Description::ComplexType::shp_t SelfDescription<DefaultObjects::CommParameterResponse>(
    DefaultObjects::CommParameterResponse *obj,
    std::string name) {

  SeRoNet::CommunicationObjects::Description::ComplexType::shp_t ret(
      new SeRoNet::CommunicationObjects::Description::ComplexType(name)
  );

  //  Add Members
//  (*ret).add(
//      SeRoNet::CommunicationObjects::Description::IDescription::shp_t(
//          new SeRoNet::CommunicationObjects::Description::ElementInt32(
//              "x",
//              std::bind(&CoordinateObject::getX, obj),
//              std::bind(&CoordinateObject::setX, obj, std::placeholders::_1)
//          )
//      )
//  ).add(
//      SeRoNet::CommunicationObjects::Description::IDescription::shp_t(
//          new SeRoNet::CommunicationObjects::Description::ElementInt32(
//              "y",
//              std::bind(&CoordinateObject::getY, obj),
//              std::bind(&CoordinateObject::setY, obj, std::placeholders::_1)
//          )
//      )
//  ).add(
//      SeRoNet::CommunicationObjects::Description::IDescription::shp_t(
//          new SeRoNet::CommunicationObjects::Description::ElementInt32(
//              "z",
//              std::bind(&CoordinateObject::getZ, obj),
//              std::bind(&CoordinateObject::setZ, obj, std::placeholders::_1)
//          )
//      )
//  );
  return ret;
}

}
}
}

#endif //SERONETSDK_PARAMETERRESPONSEDESCRIPTION_H
