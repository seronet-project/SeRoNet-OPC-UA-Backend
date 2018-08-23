//
// Created by ac120371 on 23.08.2018.
//

#include <functional>
#include "WiringCommObjectDescription.hpp"
#include "../ComplexType.hpp"
#include "../ElementPrimitives.hpp"

namespace SeRoNet {
namespace CommunicationObjects {
namespace Description {

template<>
IDescription::shp_t SelfDescription(DefaultObjects::WiringCommObject *obj,
                                    std::string name) {

  ComplexType::shp_t ret(
      new ComplexType(name)
  );

  // Add Members
  (*ret).add(
      IDescription::shp_t(
          new ElementPrimitive<std::string>(
              "command",
              std::bind(&DefaultObjects::WiringCommObject::getCommandType, obj),
              std::bind(&DefaultObjects::WiringCommObject::setCommandType,
                        obj,
                        std::placeholders::_1)
          )
      )
  ).add(
      IDescription::shp_t(
          new ElementPrimitive<std::string>(
              "slaveport",
              std::bind(&DefaultObjects::WiringCommObject::getSlaveport, obj),
              std::bind(&DefaultObjects::WiringCommObject::setSlaveport,
                        obj,
                        std::placeholders::_1)
          )
      )
  ).add(
      IDescription::shp_t(
          new ElementPrimitive<std::string>(
              "servercomponent",
              std::bind(&DefaultObjects::WiringCommObject::getServercomponent, obj),
              std::bind(&DefaultObjects::WiringCommObject::setServercomponent,
                        obj,
                        std::placeholders::_1)
          )
      )
  ).add(
      IDescription::shp_t(
          new ElementPrimitive<std::string>(
              "serverservice",
              std::bind(&DefaultObjects::WiringCommObject::getServerservice, obj),
              std::bind(&DefaultObjects::WiringCommObject::setServerservice,
                        obj,
                        std::placeholders::_1)
          )
      )
  ).add(
      IDescription::shp_t(
          new ElementPrimitive<std::int32_t>(
              "status",
              std::bind(static_cast<std::int32_t(DefaultObjects::WiringCommObject::*)()>(&DefaultObjects::WiringCommObject::getStatus),
                        obj),
              std::bind(static_cast<void (DefaultObjects::WiringCommObject::*)(std::int32_t)>(&DefaultObjects::WiringCommObject::setStatus),
                        obj,
                        std::placeholders::_1)
          )
      )
  );

  return IDescription::shp_t();
}

} // end namespace Description
} // end namespace CommunicationObjects
} // end namespace SeRoNet
