///
/// \file ParameterResponseIdlDescription.hpp
/// \author Christian von Arnim
/// \date 15.08.2018
///

#pragma once
#include "../SelfDescription.hpp"
#include "../../DefaultObjects/CommParameterIDL/ParameterResponseIdl.hpp"

namespace SeRoNet {
namespace CommunicationObjects {
namespace Description {

template<>
CommunicationObjects::Description::IDescription::shp_t SelfDescription<
    DefaultObjects::CommParameterIDL::CommParameterResponse>(
    DefaultObjects::CommParameterIDL::CommParameterResponse *obj,
    std::string name);

}
}
}
