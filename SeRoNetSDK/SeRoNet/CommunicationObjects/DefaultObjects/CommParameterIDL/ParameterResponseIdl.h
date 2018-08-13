//
// Created by ac120371 on 09.08.2018.
//

#ifndef SERONETSDK_PARAMETERRESPONSEIDL_H
#define SERONETSDK_PARAMETERRESPONSEIDL_H

#include "../EnumParamResponseType.h"

namespace SeRoNet {
namespace CommunicationObjects {
namespace DefaultObjects {
namespace CommParameterIDL {

struct CommParameterResponse {
  ENUM_ParamResponseType response;
};

} // end namespace CommParameterIDL
} // end namespace DefaultObjects
} // end namespace CommunicationObjects
} // end namespace SeRoNet

#endif //SERONETSDK_PARAMETERRESPONSEIDL_H
