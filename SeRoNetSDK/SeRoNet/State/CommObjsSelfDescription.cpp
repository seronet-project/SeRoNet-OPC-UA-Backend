/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/


#include "CommObjsSelfDescription.hpp"

namespace SeRoNet {
namespace CommunicationObjects {
namespace Description {

template<>
IDescription::shp_t SelfDescription(SeRoNet::State::CommStateResponse *obj, std::string name) {
  auto ret = std::make_shared<Description::ComplexType>(name);

  ret->add(
      std::make_shared<ElementInt32>(
          "status",
          std::bind(&SeRoNet::State::CommStateResponse::getStatus, obj),
          std::bind(&SeRoNet::State::CommStateResponse::setStatus, obj, std::placeholders::_1)
      )
  );

  ret->add(
      std::make_shared<ArrayOf<std::string>>(
          "states",
          std::bind(&SeRoNet::State::CommStateResponse::getStateList, obj),
          std::bind(&SeRoNet::State::CommStateResponse::setStateList, obj, std::placeholders::_1)
      )
  );

  return ret;
}

template<>
IDescription::shp_t SelfDescription(SeRoNet::State::CommStateRequest *obj, std::string name) {
  auto ret = std::make_shared<Description::ComplexType>(name);

  ret->add(
      std::make_shared<ElementInt32>(
          "command",
          [obj]() { return static_cast<int>(obj->getCommand()); },
          [obj](int v) { obj->setCommand(static_cast<SeRoNet::State::StateCommand>(v)); }
          //std::bind(&SeRoNet::State::CommStateRequest::getCommand, obj),
          //std::bind(&SeRoNet::State::CommStateRequest::setCommand, obj, std::placeholders::_1)
      )
  );

  ret->add(
      std::make_shared<ElementString>(
          "state_name",
          std::bind(&SeRoNet::State::CommStateRequest::getStateName, obj),
          std::bind(&SeRoNet::State::CommStateRequest::setStateName, obj, std::placeholders::_1)
      )
  );

  return ret;
}

}
}
}
