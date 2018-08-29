//
// Created by ac120371 on 09.08.2018.
//

#ifndef SERONETSDK_PARAMETERRESPONSEIDL_H
#define SERONETSDK_PARAMETERRESPONSEIDL_H

#include "../EnumParamResponseType.hpp"

namespace SeRoNet {
namespace DefaultCommObjects {
namespace CommParameterIDL {


struct CommParameterResponse {
  SeRoNet::DefaultCommObjects::ENUM_ParamResponseType response;
};

} // end namespace CommParameterIDL
} // end namespace DefaultCommObjects
} // end namespace SeRoNet

#endif //SERONETSDK_PARAMETERRESPONSEIDL_H
