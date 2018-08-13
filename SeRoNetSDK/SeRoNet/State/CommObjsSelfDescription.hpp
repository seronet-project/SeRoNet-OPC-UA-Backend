///
/// \file CommObjsSelfDescription.hpp
/// \author Christian von Arnim
/// \date 01.08.2018
///

#pragma once
#include "../CommunicationObjects/Description/SelfDescription.hpp"
#include "../CommunicationObjects/Description/ElementPrimitives.hpp"
#include "../CommunicationObjects/Description/ElementArray.hpp"
#include "../CommunicationObjects/Description/ComplexType.hpp"

#include "CommObjs.hpp"

namespace SeRoNet {
namespace CommunicationObjects {
namespace Description {

template<>
IDescription::shp_t SelfDescription(SeRoNet::State::CommStateResponse *obj, std::string name);

template<>
IDescription::shp_t SelfDescription(SeRoNet::State::CommStateRequest *obj, std::string name);

}
}
}
