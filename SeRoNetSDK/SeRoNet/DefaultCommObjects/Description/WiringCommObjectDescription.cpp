//
// Created by ac120371 on 23.08.2018.
//

#include <functional>
#include "WiringCommObjectDescription.hpp"
#include "../../CommunicationObjects/Description/ComplexType.hpp"
#include "../../CommunicationObjects/Description/ElementPrimitives.hpp"

template<>
SeRoNet::CommunicationObjects::Description::IDescription::shp_t SeRoNet::CommunicationObjects::Description::SelfDescription(
    DefaultCommObjects::WiringCommObject *obj,

    std::string name) {

  ComplexType::shp_t ret(
      new ComplexType(name)
  );

  // Add Members
  (*ret).add(
      IDescription::shp_t(
          new ElementPrimitive<std::string>(
              "command",
              std::bind(&SeRoNet::DefaultCommObjects::WiringCommObject::getCommandType, obj),
              std::bind(&SeRoNet::DefaultCommObjects::WiringCommObject::setCommandType,
                        obj,
                        std::placeholders::_1)
          )
      )
  ).add(
      IDescription::shp_t(
          new ElementPrimitive<std::string>(
              "slaveport",
              std::bind(&SeRoNet::DefaultCommObjects::WiringCommObject::getSlaveport, obj),
              std::bind(&SeRoNet::DefaultCommObjects::WiringCommObject::setSlaveport,
                        obj,
                        std::placeholders::_1)
          )
      )
  ).add(
      IDescription::shp_t(
          new ElementPrimitive<std::string>(
              "servercomponent",
              std::bind(&SeRoNet::DefaultCommObjects::WiringCommObject::getServercomponent, obj),
              std::bind(&SeRoNet::DefaultCommObjects::WiringCommObject::setServercomponent,
                        obj,
                        std::placeholders::_1)
          )
      )
  ).add(
      IDescription::shp_t(
          new ElementPrimitive<std::string>(
              "serverservice",
              std::bind(&SeRoNet::DefaultCommObjects::WiringCommObject::getServerservice, obj),
              std::bind(&SeRoNet::DefaultCommObjects::WiringCommObject::setServerservice,
                        obj,
                        std::placeholders::_1)
          )
      )
  ).add(
      IDescription::shp_t(
          new ElementPrimitive<std::int32_t>(
              "status",
              std::bind(static_cast<std::int32_t (SeRoNet::DefaultCommObjects::WiringCommObject::*)()>(&SeRoNet::DefaultCommObjects::WiringCommObject::getStatus),
                        obj),
              std::bind(static_cast<void (SeRoNet::DefaultCommObjects::WiringCommObject::*)(std::int32_t)>(&SeRoNet::DefaultCommObjects::WiringCommObject::setStatus),
                        obj,
                        std::placeholders::_1)
          )
      )
  );

  return ret;
}
